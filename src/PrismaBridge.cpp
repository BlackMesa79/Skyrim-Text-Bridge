#include "PrismaBridge.h"
#ifndef TEXTBRIDGE_PRISMA_TEST
#include <SKSE/SKSE.h>
#endif
#include <PrismaUI_API.h>
#include <atomic>
namespace TextBridge::Prisma {
namespace { PRISMA_UI_API::IVPrismaUI1* api{}; bool unavailable = false; std::atomic<bool> focused{false}; }
bool Install() {
    api = nullptr; unavailable = false; focused = false;
    const auto module = GetModuleHandleW(L"PrismaUI.dll");
    if (!module) return true;
    // Negotiate the public interface version, not the framework release number.
    // Prisma returns nullptr if V1 is unsupported; never call a private vtable.
    api = PRISMA_UI_API::RequestPluginAPI<PRISMA_UI_API::IVPrismaUI1>();
    unavailable = !api;
    focused = unavailable;
    SKSE::log::info("Prisma focus query: {}", api ? "ready (read-only)" : "unavailable; direct IME disabled");
    return !unavailable;
}
// This API may wait for Ultralight: never call it from the window procedure.
void Refresh() { focused = unavailable || (api && api->HasAnyActiveFocus()); }
void Relinquish() { focused = true; }
bool OwnsInput() { return focused.load(); }
}
