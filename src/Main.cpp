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
namespace {
using Dispatch = void(RE::BSTEventSource<RE::InputEvent*>*, RE::InputEvent**);
REL::Relocation<Dispatch> previousDispatch;
HWND gameWindow{}; bool installed = false;
TextBridge::TypingGuard typingGuard;
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
    if (GetForegroundWindow() != gameWindow) typingGuard.Clear();
    if (!settings.enabled || SettingsMenu::Blocking()) {
        typingGuard.Clear();
        if (DirectIme::Active()) DirectIme::Reset();
        if (Meridian::Session()) Meridian::Cancel();
        previousDispatch(source,events); return;
    }
    bool toggleDown = false;
    for (auto e = events ? *events : nullptr; e; e = e->next)
        if (auto b = e->AsButtonEvent(); b && b->GetDevice() == RE::INPUT_DEVICE::kKeyboard && b->GetIDCode() == hotkey && b->IsDown()) toggleDown = true;
    // Idle path must not query Prisma, replace event lists, or touch the game HWND.
    if (!DirectIme::Active() && !Meridian::Session() && !toggleDown && !typingGuard.Pending()) {
        for (auto e = events ? *events : nullptr; e; e=e->next)
            if (auto b=e->AsButtonEvent(); b && b->GetDevice()==RE::INPUT_DEVICE::kKeyboard)
                typingGuard.Filter(b->GetIDCode(),b->IsUp(),false);
        previousDispatch(source, events); return;
    }
    Prisma::Refresh();
    if (Prisma::OwnsInput()) {
        if (toggleDown) SKSE::log::info("IME toggle bypassed: Prisma owns input or its V1 focus API is unavailable");
        typingGuard.Clear();
        if (DirectIme::Active()) DirectIme::Reset();
        if (Meridian::Session()) Meridian::Cancel();
        previousDispatch(source, events); return;
    }
    DirectIme::Tick();
    if (Meridian::Session() && (!DirectIme::Active() || !Meridian::Valid())) {
        DirectIme::Reset(); Meridian::Cancel();
    } else if (DirectIme::Active() && !Meridian::Session() && Meridian::HasFocus()) {
        DirectIme::Reset();
    }
    std::vector<std::pair<RE::InputEvent*, RE::InputEvent*>> links;
    RE::InputEvent* head = nullptr; RE::InputEvent** tail = &head;
    const bool wasDirect = DirectIme::SuppressKeyboard();
    for (auto e = events ? *events : nullptr; e;) {
        auto next = e->next; links.emplace_back(e, next);
        const auto b = e->AsButtonEvent();
        const bool toggle = b && b->GetDevice() == RE::INPUT_DEVICE::kKeyboard && b->GetIDCode() == hotkey;
        if (toggle && b->IsDown()) {
            if (!DirectIme::Active() && Meridian::HasFocus() && !Meridian::Begin()) { e=next; continue; }
            const bool on = DirectIme::Toggle();
            if (!on) Meridian::Cancel();
            SKSE::log::info("Direct IME {}", on ? "enabled" : "disabled");
        }
        const bool suppress = wasDirect || DirectIme::SuppressKeyboard();
        const auto scan = b ? b->GetIDCode() : 0;
        const bool keyboard = b && b->GetDevice() == RE::INPUT_DEVICE::kKeyboard;
        const bool language = keyboard && scan == 0x39 && suppress && (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        if (language && b->IsDown()) DirectIme::ToggleEnglish();
        const bool consume = toggle || language || (suppress && (!EditingKey(scan) || DirectIme::BlockNavigation()));
        const bool drop = (keyboard && typingGuard.Filter(scan,b->IsUp(),consume)) || (suppress && e->AsCharEvent());
        if (!drop) { *tail = e; tail = &e->next; }
        e = next;
    }
    *tail = nullptr;
    if (Meridian::Session()) {
        Meridian::Commit(DirectIme::Take(1024));
    } else if (auto queue = RE::BSInputEventQueue::GetSingleton()) {
        const auto used = queue->charEventCount;
        const auto capacity = used < queue->MAX_CHAR_EVENTS ? queue->MAX_CHAR_EVENTS - used : 0;
        auto& data = queue->GetRuntimeData();
        const auto originalHead = data.queueHead;
        const auto originalTail = data.queueTail;
        data.queueHead = nullptr; data.queueTail = nullptr;
        for (auto c : DirectIme::Take(capacity)) queue->AddCharEvent(c);
        *tail = data.queueHead;
        data.queueHead = originalHead; data.queueTail = originalTail;
    }
    previousDispatch(source, &head);
    for (auto [node, next] : links) node->next = next;
}
class MenuEvents : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent*, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
        TextBridge::DirectIme::Reset();
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
        // CommonLib selects SE ID 67315 or AE ID 68617; both call offsets are 0x7B.
        const auto call = REL::RelocationID(67315, 68617).address() + 0x7B;
        if (*reinterpret_cast<const std::uint8_t*>(call) != 0xE8 || !TextBridge::DirectIme::Install(gameWindow)) {
            SKSE::log::error("Direct IME installation failed"); return;
        }
        SKSE::AllocTrampoline(32);
        previousDispatch = SKSE::GetTrampoline().write_call<5>(call, InputHook);
        installed = true;
        if (auto ui = RE::UI::GetSingleton()) ui->AddEventSink(new MenuEvents);
        SKSE::log::info("Direct IME installed: toggle scan code {}; Prisma native passthrough", TextBridge::Settings::Get().hotkey);
    }
    if (m->type == SKSE::MessagingInterface::kPreLoadGame || m->type == SKSE::MessagingInterface::kNewGame)
        TextBridge::DirectIme::Reset();
}
}
SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    const auto runtime=skse->RuntimeVersion();
    SKSE::log::info("Loading Text Bridge on Skyrim {}",runtime.string());
    if (!TextBridge::SupportedRuntime(runtime[0],runtime[1],runtime[2],runtime[3])) {
        SKSE::log::error("Unsupported Skyrim runtime {}; allowed: 1.5.97 and 1.6.1170",runtime.string()); return false;
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
        value.hotkey=GetPrivateProfileIntW(L"Input",L"HotkeyScanCode",0x42,config.c_str());
        TextBridge::Settings::Set(value);
    }
    return SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
}
