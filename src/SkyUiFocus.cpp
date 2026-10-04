#include <SKSE/SKSE.h>
#include <RE/C/ControlMap.h>
#include <RE/U/UI.h>
#include <RE/G/GFxMovieView.h>
#include <RE/G/GFxValue.h>
#include "SkyUiFocus.h"
namespace TextBridge::SkyUiFocus {
std::string Target() {
    auto controls = RE::ControlMap::GetSingleton();
    auto ui = RE::UI::GetSingleton();
    if (!controls || !ui || controls->GetRuntimeData().textEntryCount <= 0) return {};
    // Limit automatic input to the SkyUI item-menu family; never infer it from
    // an open menu alone. SearchWidget pairs AllowTextInput with Selection focus.
    for (const char* name : {"InventoryMenu", "MagicMenu", "ContainerMenu", "BarterMenu"}) {
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
        return "sky:" + std::string(name) + ":" + path;
    }
    return {};
}
}
