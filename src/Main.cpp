#include <SKSE/SKSE.h>
#include <RE/B/BSInputEventQueue.h>
#include <RE/M/MenuOpenCloseEvent.h>
#include <RE/U/UI.h>
#include "DirectIme.h"
#include "PrismaBridge.h"
#include "Settings.h"
#include "MeridianBridge.h"
#include "RuntimeSupport.h"
#include "TypingGuard.h"
#include "AutoInputPolicy.h"
#include "FrameworkFocus.h"
#include "GameCharacterQueue.h"
#include "ImePanel.h"
#include "ModexMenuGuard.h"
namespace {
using Dispatch = void(RE::BSTEventSource<RE::InputEvent*>*, RE::InputEvent**);
REL::Relocation<Dispatch> previousDispatch;
HWND gameWindow{}; bool installed = false;
TextBridge::TypingGuard typingGuard;
TextBridge::AutoInputPolicy autoInput;
bool modexSession = false;
BOOL CALLBACK FindWindow(HWND window, LPARAM) {
    DWORD process{}; GetWindowThreadProcessId(window, &process);
    wchar_t name[80]{}; GetClassNameW(window, name, 80);
    if (process == GetCurrentProcessId() && std::wstring_view(name) == L"Skyrim Special Edition") {
        gameWindow = window; return FALSE;
    }
    return TRUE;
}
void InputHook(RE::BSTEventSource<RE::InputEvent*>* source, RE::InputEvent** events) {
    using namespace TextBridge;
    for (auto e = events ? *events : nullptr; e; e=e->next)
        if (auto b=e->AsButtonEvent(); b && b->GetDevice()==RE::INPUT_DEVICE::kKeyboard && b->IsDown() && SettingsMenu::CaptureKey(b->GetIDCode())) {
            RE::InputEvent* empty=nullptr; previousDispatch(source,&empty); return;
        }
    const auto settings=Settings::Get();
    const unsigned hotkey=settings.hotkey;
    ModexMenuGuard::Refresh();
    if (modexSession) {
        auto ui = RE::UI::GetSingleton();
        if (!ui || !ui->IsMenuOpen(ModexMenuGuard::MenuName)) {
            DirectIme::Reset(); modexSession = false;
        }
    }
    if (!settings.autoEnable) {
        FrameworkFocus::Poll(false);
        if (!autoInput.OwnsMode()) autoInput.Reset();
    }
    if (GetForegroundWindow() != gameWindow) { typingGuard.Clear(); ModexMenuGuard::Clear(); }
    if (!settings.enabled || SettingsMenu::Blocking()) {
        FrameworkFocus::Poll(false);
        autoInput.Reset();
        typingGuard.Clear();
        if (DirectIme::Active()) DirectIme::Reset();
        modexSession = false; ModexMenuGuard::Clear();
        if (Meridian::Session()) Meridian::Cancel();
        previousDispatch(source,events); return;
    }
    bool toggleDown = false, pointerDown = false;
    for (auto e = events ? *events : nullptr; e; e = e->next) {
        if (auto b = e->AsButtonEvent(); b && b->IsDown()) {
            if (b->GetDevice() == RE::INPUT_DEVICE::kKeyboard && b->GetIDCode() == hotkey) toggleDown = true;
            if (b->GetDevice() == RE::INPUT_DEVICE::kMouse && b->GetIDCode() < 8) pointerDown = true;
        }
    }
    // With automatic input off, retain the original untouched idle path.
    if (!settings.autoEnable && !autoInput.OwnsMode() && !DirectIme::Active() && !Meridian::Session() && !toggleDown && !typingGuard.Pending()) {
        for (auto e = events ? *events : nullptr; e; e=e->next)
            if (auto b=e->AsButtonEvent(); b && b->GetDevice()==RE::INPUT_DEVICE::kKeyboard)
                { typingGuard.Filter(b->GetIDCode(),b->IsUp(),false);
                  if (b->GetIDCode() == 0x01) ModexMenuGuard::ObserveEscape(b->IsUp(), false); }
        previousDispatch(source, events); return;
    }
    Prisma::Refresh();
    if (Prisma::OwnsInput()) {
        FrameworkFocus::Poll(false);
        autoInput.Reset();
        if (toggleDown) SKSE::log::info("IME toggle bypassed: Prisma owns input or its V1 focus API is unavailable");
        typingGuard.Clear();
        modexSession = false; ModexMenuGuard::Clear();
        if (DirectIme::Active()) DirectIme::Reset();
        if (Meridian::Session()) Meridian::Cancel();
        previousDispatch(source, events); return;
    }
    const bool wasDirect = DirectIme::SuppressKeyboard();
    DirectIme::Tick();
    if (Meridian::Session() && (!DirectIme::Active() || !Meridian::Valid())) {
        DirectIme::Reset(); Meridian::Cancel();
    } else if (DirectIme::Active() && !Meridian::Session() && Meridian::HasFocus()) {
        DirectIme::Reset();
    }
    // Mouse focus changes are dispatched below. End the old automatic session
    // first so queued text cannot follow a click into another Scaleform field.
    const bool deferAutoStart = pointerDown && autoInput.OwnsMode();
    if (deferAutoStart) {
        DirectIme::Reset(); Meridian::Cancel(); autoInput.Interrupted();
    }
    const bool autoPermitted = settings.autoEnable && GetForegroundWindow() == gameWindow;
    FrameworkFocus::Poll(autoPermitted);
    const auto focus = autoPermitted ? FrameworkFocus::Current() : FrameworkFocus::Target{};
    const auto& target = focus.identity;
    const auto action = autoInput.Update(target, autoPermitted, DirectIme::SuppressKeyboard(), toggleDown || deferAutoStart);
    if (action == AutoInputPolicy::Action::Stop) {
        DirectIme::Reset(); Meridian::Cancel();
    } else if (action == AutoInputPolicy::Action::Start && !DirectIme::Active()) {
        if (!FrameworkFocus::Begin(focus)) autoInput.Failed(target);
        else if (DirectIme::Toggle()) autoInput.Started(target);
        else { Meridian::Cancel(); autoInput.Failed(target); }
    }
    std::vector<std::pair<RE::InputEvent*, RE::InputEvent*>> links;
    RE::InputEvent* head = nullptr; RE::InputEvent** tail = &head;
    for (auto e = events ? *events : nullptr; e;) {
        auto next = e->next; links.emplace_back(e, next);
        const auto b = e->AsButtonEvent();
        const auto scan = b ? b->GetIDCode() : 0;
        const bool keyboard = b && b->GetDevice() == RE::INPUT_DEVICE::kKeyboard;
        const bool escape = keyboard && scan == 0x01 && (wasDirect || DirectIme::SuppressKeyboard());
        // IMM teardown may re-enter UI message delivery before Escape returns.
        if (escape && b->IsDown()) ModexMenuGuard::ObserveEscape(false, true);
        if (escape && b->IsDown() && DirectIme::Escape()) {
            Meridian::Cancel();
            if (autoPermitted) autoInput.Manual(target, false);
        }
        const bool toggle = b && b->GetDevice() == RE::INPUT_DEVICE::kKeyboard && b->GetIDCode() == hotkey;
        if (toggle && b->IsDown()) {
            if (!wasDirect && !DirectIme::Active() && Meridian::HasFocus() && !Meridian::Begin()) {
                if (autoPermitted) autoInput.Failed(target);
                e=next; continue;
            }
            // Focus loss may already have closed an active automatic session this frame.
            const bool on = wasDirect && !DirectIme::SuppressKeyboard() ? false : DirectIme::Toggle();
            auto ui = RE::UI::GetSingleton();
            modexSession = on && ui && ui->IsMenuOpen(ModexMenuGuard::MenuName);
            if (!on) Meridian::Cancel();
            if (autoPermitted) autoInput.Manual(target, on);
            SKSE::log::info("Direct IME {}", on ? "enabled" : "disabled");
        }
        const bool suppress = wasDirect || DirectIme::SuppressKeyboard();
        const bool language = keyboard && scan == 0x39 && suppress && (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        if (language && b->IsDown()) DirectIme::ToggleEnglish();
        const bool shortcut=EditingShortcut(scan,(GetAsyncKeyState(VK_CONTROL)&0x8000)!=0);
        const bool consume = toggle || escape || language || (suppress && ((!EditingKey(scan) && !shortcut) || DirectIme::BlockNavigation()));
        const bool drop = (keyboard && typingGuard.Filter(scan,b->IsUp(),consume)) || (suppress && e->AsCharEvent());
        if (keyboard && scan == 0x01) ModexMenuGuard::ObserveEscape(b->IsUp(), drop);
        if (!drop) { *tail = e; tail = &e->next; }
        e = next;
    }
    *tail = nullptr;
    if (Meridian::Session()) {
        Meridian::Commit(DirectIme::Take(1024));
    } else if (auto queue = RE::BSInputEventQueue::GetSingleton()) {
        const auto capacity = AvailableCharacters(*queue);
        // 1.7.x has additional event pools before these links. Use the library's
        // runtime-aware accessors for both legacy and experimental runtimes.
        *tail = AppendGameCharacters(*queue, DirectIme::Take(capacity));
    }
    previousDispatch(source, &head);
    for (auto [node, next] : links) node->next = next;
}
class MenuEvents : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
        TextBridge::ModexMenuGuard::Refresh();
        if (event && event->menuName == TextBridge::ModexMenuGuard::MenuName && !event->opening) modexSession = false;
        TextBridge::DirectIme::Reset();
        TextBridge::Meridian::Cancel();
        autoInput.Interrupted();
        typingGuard.Clear();
        return RE::BSEventNotifyControl::kContinue;
    }
};
void OnMessage(SKSE::MessagingInterface::Message* m) {
    if (!m) return;
    if (m->type == SKSE::MessagingInterface::kPostPostLoad) TextBridge::Prisma::Install();
    if (m->type == SKSE::MessagingInterface::kDataLoaded && !installed) {
        if (GetModuleHandleW(L"SimpleIME.dll")) {
            SKSE::log::error("Disable SimpleIME before using direct IME"); return;
        }
        EnumWindows(FindWindow, 0);
        TextBridge::Meridian::Install();
        TextBridge::SettingsMenu::Install();
        if (!TextBridge::ImePanel::Install()) {
            SKSE::log::error("Input disabled: install a compatible SKSE Menu Framework for the ImGui candidate panel");
            return;
        }
        // Matches Meridian's current guarded dispatch call site. The 1.7.99/104
        // sites still need game verification; never install if it is not E8.
        const auto call = REL::RelocationID(67315, 68617).address() + 0x7B;
        if (*reinterpret_cast<const std::uint8_t*>(call) != 0xE8 || !TextBridge::DirectIme::Install(gameWindow)) {
            SKSE::log::error("Direct IME installation failed"); return;
        }
        SKSE::AllocTrampoline(32);
        previousDispatch = SKSE::GetTrampoline().write_call<5>(call, InputHook);
        installed = true;
        SKSE::log::info("Text Bridge 0.3.12-beta.7; framework focus routing (Scaleform/Meridian, Prisma native); ImGui HUD panel; CommonLibSSE-NG 10.1.0; input hook installed on {} at {:X}", REL::Module::get().version().string(), call);
        if (auto ui = RE::UI::GetSingleton()) ui->AddEventSink(new MenuEvents);
        SKSE::log::info("Direct IME installed: toggle scan code {}; Prisma native passthrough", TextBridge::Settings::Get().hotkey);
    }
    if (m->type == SKSE::MessagingInterface::kPreLoadGame || m->type == SKSE::MessagingInterface::kNewGame) {
        TextBridge::DirectIme::Reset();
        TextBridge::Meridian::Cancel();
        TextBridge::FrameworkFocus::Poll(false);
        autoInput.Reset(); typingGuard.Clear();
        modexSession = false; TextBridge::ModexMenuGuard::Clear();
    }
}
}
SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    const auto runtime=skse->RuntimeVersion();
    SKSE::log::info("Loading Text Bridge on Skyrim {}",runtime.string());
    if (!TextBridge::SupportedRuntime(runtime[0],runtime[1],runtime[2],runtime[3])) {
        SKSE::log::error("Unsupported Skyrim runtime {}; allowed: 1.5.97, 1.6.1170, 1.7.99 and 1.7.104",runtime.string()); return false;
    }
    if (GetModuleHandleW(L"SimpleIME.dll")) {
        SKSE::log::error("Disable SimpleIME before loading Text Bridge"); return false;
    }
    if (!SKSE::GetTaskInterface() || !SKSE::GetMessagingInterface()) return false;
    const auto config = std::filesystem::absolute("Data/SKSE/Plugins/SkyrimTextBridge.ini");
    const auto userConfig = std::filesystem::absolute("Data/SKSE/Plugins/SkyrimTextBridge.user.ini");
    const bool hasUser=std::filesystem::exists(userConfig);
    TextBridge::Settings::Load(userConfig.wstring());
    if (!hasUser) {
        auto value=TextBridge::Settings::Get();
        value.autoEnable=GetPrivateProfileIntW(L"Input",L"AutoEnable",0,config.c_str())!=0;
        value.hotkey=GetPrivateProfileIntW(L"Input",L"HotkeyScanCode",0x42,config.c_str());
        TextBridge::Settings::Set(value);
    }
    return SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
}
