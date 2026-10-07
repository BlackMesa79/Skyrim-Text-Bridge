#ifndef TEXTBRIDGE_FOCUS_TEST
#include <SKSE/SKSE.h>
#include <RE/C/ControlMap.h>
#include <RE/U/UI.h>
#include <RE/G/GFxMovieView.h>
#include <RE/G/GFxValue.h>
#endif
#include "SkyUiFocus.h"
namespace TextBridge::SkyUiFocus {
std::string Target() {
    auto controls = RE::ControlMap::GetSingleton();
    auto ui = RE::UI::GetSingleton();
    if (!controls || !ui || controls->GetRuntimeData().textEntryCount <= 0) return {};
    // Require both the engine's text-input permission and a focused editable
    // field. RaceMenu shares the engine RaceSex Menu and enables text input for
    // its naming dialog; opening character customization alone is insufficient.
    for (const char* name : {"InventoryMenu", "MagicMenu", "ContainerMenu", "BarterMenu", "RaceSex Menu"}) {
        if (!ui->IsMenuOpen(name)) continue;
        auto movie = ui->GetMovieView(name); if (!movie) continue;
        RE::GFxValue focus;
        if (!movie->Invoke("Selection.getFocus", &focus, nullptr, 0) || !focus.IsString()) {
            if (!movie->Invoke("_global.Selection.getFocus", &focus, nullptr, 0) || !focus.IsString()) continue;
        }
        const char* value = focus.GetString();
        if (!value || !*value) continue;
        std::string path(value);
        if (path.size() > 512) continue;
        RE::GFxValue type, password;
        if (!movie->GetVariable(&type, (path + ".type").c_str()) || !type.IsString() ||
            std::string_view(type.GetString()) != "input") continue;
        if (movie->GetVariable(&password, (path + ".password").c_str()) && password.IsBool() && password.GetBool()) continue;
        // A dialog may retain Selection focus after being hidden. Check the
        // field and its containing clips, so a closed naming dialog cannot
        // leave automatic input active during slider editing or gameplay.
        bool unavailable = false;
        for (std::string current = path; !current.empty();) {
            RE::GFxValue property;
            if (movie->GetVariable(&property, (current + "._visible").c_str()) && property.IsBool() && !property.GetBool()) unavailable = true;
            if (movie->GetVariable(&property, (current + ".enabled").c_str()) && property.IsBool() && !property.GetBool()) unavailable = true;
            if (movie->GetVariable(&property, (current + ".disabled").c_str()) && property.IsBool() && property.GetBool()) unavailable = true;
            if (unavailable) break;
            const auto dot = current.rfind('.');
            if (dot == std::string::npos) break;
            current.resize(dot);
        }
        if (unavailable) continue;
        return "sky:" + std::string(name) + ":" + path;
    }
    return {};
}
}
