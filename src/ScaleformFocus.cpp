#ifndef TEXTBRIDGE_FOCUS_TEST
#include <SKSE/SKSE.h>
#include <RE/C/ControlMap.h>
#include <RE/U/UI.h>
#include <RE/G/GFxMovieView.h>
#include <RE/G/GFxValue.h>
#endif
#include "ScaleformFocus.h"
#include "PrismaBridge.h"
#include <vector>
#include <cstdint>
#include <algorithm>

namespace TextBridge::ScaleformFocus {
namespace {
struct Candidate {
    std::string name;
    RE::GPtr<RE::IMenu> menu;
    RE::GPtr<RE::GFxMovieView> movie;
    int depth{};
    std::size_t order{};
};
std::vector<Candidate> Snapshot(RE::UI& ui) {
    std::vector<Candidate> result;
    // Retain the menu/movie while looking up focus, but never execute
    // ActionScript while holding the engine's menu-message lock.
    RE::BSSpinLockGuard lock(ui.processMessagesLock);
    if (!ui.IsShowingMenus() || ui.closingAllMenus) return result;
    for (std::size_t i = 0; i < ui.menuStack.size(); ++i) {
        auto menu = ui.menuStack[i];
        if (!menu || !menu->OnStack()) continue;
        const auto context = menu->inputContext.get();
        const bool inputRole = menu->UsesMenuContext() || menu->Modal() ||
            (context != RE::IMenu::Context::kNone && context != RE::IMenu::Context::kGameplay &&
             context != RE::IMenu::Context::kCursor && context != RE::IMenu::Context::kDebugOverlay);
        // CustomRendering describes drawing, not keyboard ownership or the
        // absence of a Scaleform movie. Inventory menus also carry this flag.
        // Native cursor/HUD/fader layers have no input role, do not pause, and
        // are persistent, cursor-assigned or saving-compatible render layers.
        const bool passive = !inputRole && !menu->PausesGame() && !menu->InventoryItemMenu() &&
            !menu->ApplicationMenu() && (menu->AlwaysOpen() || menu->AssignCursorToRenderer() ||
                (menu->CustomRendering() && menu->AllowSaving()));
        if (passive) continue;
        const bool ownsInput = inputRole || menu->InventoryItemMenu() || menu->PausesGame() ||
            menu->ApplicationMenu() || menu->CustomRendering();
        if (!ownsInput) continue;
        std::string name;
        for (const auto& entry : ui.menuMap)
            if (entry.second.menu.get() == menu.get()) { name = entry.first.c_str(); break; }
        result.push_back({std::move(name), menu, menu->uiMovie, menu->depthPriority, i});
    }
    return result;
}
const Candidate* Top(const std::vector<Candidate>& menus) {
    if (menus.empty()) return nullptr;
    return &*std::max_element(menus.begin(), menus.end(), [](const Candidate& a, const Candidate& b) {
        return a.depth < b.depth || (a.depth == b.depth && a.order < b.order);
    });
}
std::string Field(RE::GFxMovieView& movie) {
    if (!movie.GetVisible()) return {};
    RE::GFxValue focus;
    if (!movie.Invoke("Selection.getFocus", &focus, nullptr, 0) || !focus.IsString()) {
        if (!movie.Invoke("_global.Selection.getFocus", &focus, nullptr, 0) || !focus.IsString()) return {};
    }
    const char* value = focus.GetString();
    if (!value || !*value) return {};
    std::string path(value);
    if (path.size() > 512 || std::any_of(path.begin(), path.end(), [](unsigned char c) { return c < 0x20 || c == 0x7f; })) return {};
    RE::GFxValue type;
    if (!movie.GetVariable(&type, (path + ".type").c_str()) || !type.IsString() ||
        std::string_view(type.GetString()) != "input") return {};
    // Missing optional properties do not invalidate AS2 TextFields. Explicit
    // password/readonly, hidden or disabled fields and ancestors do.
    for (std::string current = path; !current.empty();) {
        for (const char* property : {"password", "readOnly", "disabled"}) {
            RE::GFxValue flag;
            if (movie.GetVariable(&flag, (current + "." + property).c_str()) && flag.IsBool() && flag.GetBool()) return {};
        }
        for (const char* property : {"_visible", "enabled", "editable"}) {
            RE::GFxValue flag;
            if (movie.GetVariable(&flag, (current + "." + property).c_str()) && flag.IsBool() && !flag.GetBool()) return {};
        }
        const auto dot = current.rfind('.');
        if (dot == std::string::npos) break;
        current.resize(dot);
    }
    return path;
}
}
bool ConsoleOpen() {
    auto ui = RE::UI::GetSingleton();
    if (!ui) return false;
    RE::BSSpinLockGuard lock(ui->processMessagesLock);
    for (const auto& menu : ui->menuStack) {
        if (!menu || !menu->OnStack()) continue;
        if (menu->inputContext.get() == RE::IMenu::Context::kConsole) return true;
        for (const auto& entry : ui->menuMap)
            if (entry.second.menu.get() == menu.get() && std::string_view(entry.first.c_str()) == "Console") return true;
    }
    return false;
}
std::string Target() {
    if (Prisma::OwnsInput() || ConsoleOpen()) return {};
    auto controls = RE::ControlMap::GetSingleton();
    auto ui = RE::UI::GetSingleton();
    if (!controls || !ui || controls->GetRuntimeData().textEntryCount <= 0) return {};
    auto menus = Snapshot(*ui);
    const auto top = Top(menus);
    if (!top) return {};
    // A top owner without a supported movie/field blocks stale focus below.
    // CustomRendering alone must not reject a real Scaleform editable field.
    if (top->name.empty() || !top->movie) return {};
    const auto path = Field(*top->movie);
    if (path.empty() || Prisma::OwnsInput() || ConsoleOpen() || controls->GetRuntimeData().textEntryCount <= 0) return {};
    // AS queries may synchronously close/recreate a menu or open a dialog.
    // Confirm its ownership again using the same locked snapshot path.
    const auto currentMenus = Snapshot(*ui);
    const auto current = Top(currentMenus);
    if (!current || current->menu.get() != top->menu.get() || current->movie.get() != top->movie.get() || current->name != top->name) return {};
    return "sf:" + std::to_string(top->name.size()) + ":" + top->name + ":" +
        std::to_string(reinterpret_cast<std::uintptr_t>(top->movie.get())) + ":" + path;
}
}
