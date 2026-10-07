#ifndef TEXTBRIDGE_MODEX_TEST
#include <SKSE/SKSE.h>
#include <RE/B/BSUIMessageData.h>
#include <RE/B/BSUIScaleformData.h>
#include <RE/G/GFxEvent.h>
#include <RE/U/UI.h>
#include <RE/C/ControlMap.h>
#endif
#include <mutex>
#include "ModexMenuGuard.h"
#include "EscapeFence.h"
#include "MenuTextLease.h"
#include "DirectIme.h"
#include "PrismaBridge.h"

namespace TextBridge::ModexMenuGuard {
namespace {
EscapeFence escape;
using Process = RE::UI_MESSAGE_RESULTS(RE::IMenu*, RE::UIMessage&);
std::atomic<Process*> previous{};
std::atomic<void**> installedSlot{};
std::mutex installMutex;
using Display = void(RE::IMenu*);
std::atomic<Display*> previousDisplay{};
MenuTextLease lease;
#ifndef TEXTBRIDGE_MODEX_TEST
int TextCount() { auto map = RE::ControlMap::GetSingleton(); return map ? map->GetRuntimeData().textEntryCount : -1; }
void TextPermission(bool allow) { if (auto map = RE::ControlMap::GetSingleton()) map->AllowTextInput(allow); }
#endif
void PostDisplay(RE::IMenu* menu) {
    const auto before = TextCount();
    previousDisplay.load()(menu);
    const auto after = TextCount();
    if (before >= 0 && after >= 0) lease.Observe(before, after);
}
RE::UI_MESSAGE_RESULTS ProcessMessage(RE::IMenu* menu, RE::UIMessage& message) {
    const bool local = !Prisma::OwnsInput() &&
        (DirectIme::SuppressKeyboard() || escape.Blocks(GetTickCount64()));
    if (local && message.type == RE::UI_MESSAGE_TYPE::kUserEvent && message.data) {
        auto data = static_cast<RE::BSUIMessageData*>(message.data);
        if (data->fixedStr == "Cancel") return RE::UI_MESSAGE_RESULTS::kHandled;
    }
    if (local && message.type == RE::UI_MESSAGE_TYPE::kScaleformEvent && message.data) {
        auto event = static_cast<RE::BSUIScaleformData*>(message.data)->scaleformEvent;
        if (event && (event->type == RE::GFxEvent::EventType::kKeyDown ||
                      event->type == RE::GFxEvent::EventType::kKeyUp) &&
            static_cast<RE::GFxKeyEvent*>(event)->keyCode == RE::GFxKey::kEscape)
            return RE::UI_MESSAGE_RESULTS::kHandled;
    }
    // Restore a previously repaired lease before Modex reuses its retained
    // WantTextInput flag. Its next render can then release it normally.
    if (message.type == RE::UI_MESSAGE_TYPE::kShow && lease.Reopen(TextCount())) TextPermission(true);
    const auto before = TextCount();
    const auto result = previous.load()(menu, message);
    const auto after = TextCount();
    if (before >= 0 && after >= 0) lease.Observe(before, after);
    if (message.type == RE::UI_MESSAGE_TYPE::kHide && lease.Close(after)) TextPermission(false);
    // Never swallow hide/force-hide or change gameplay control flags.
    return result;
}
}
void Refresh() {
#ifndef TEXTBRIDGE_MODEX_TEST
    if (installedSlot) return;
    std::lock_guard lock(installMutex);
    if (installedSlot) return;
    auto module = GetModuleHandleW(L"Modex.dll");
    auto ui = RE::UI::GetSingleton();
    if (!module || !ui) return;
    auto menu = ui->GetMenu(MenuName);
    if (!menu) return;
    auto slot = *reinterpret_cast<void***>(menu.get()) + 4; // CommonLib IMenu::ProcessMessage
    auto displaySlot = slot + 2; // CommonLib IMenu::PostDisplay
    MEMORY_BASIC_INFORMATION info{};
    // Only a real Modex-owned override of the public IMenu interface is eligible.
    if (!VirtualQuery(*slot, &info, sizeof(info)) || info.AllocationBase != module ||
        !(info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))) return;
    if (!VirtualQuery(slot, &info, sizeof(info)) || info.AllocationBase != module) return;
    if (!VirtualQuery(*displaySlot, &info, sizeof(info)) || info.AllocationBase != module ||
        !(info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))) return;
    DWORD old{};
    if (!VirtualProtect(slot, 3 * sizeof(void*), PAGE_READWRITE, &old)) return;
    previous = reinterpret_cast<Process*>(*slot);
    previousDisplay = reinterpret_cast<Display*>(*displaySlot);
    InterlockedExchangePointer(displaySlot, reinterpret_cast<void*>(&PostDisplay));
    InterlockedExchangePointer(slot, reinterpret_cast<void*>(&ProcessMessage));
    DWORD ignored{}; VirtualProtect(slot, 3 * sizeof(void*), old, &ignored);
    installedSlot = slot;
    SKSE::log::info("Modex IMenu Esc/Cancel guard and scoped text-lease cleanup installed");
#endif
}
void ObserveEscape(bool up, bool consumed) { escape.Observe(up, consumed, GetTickCount64()); }
void Clear() { escape.Clear(); }
}
