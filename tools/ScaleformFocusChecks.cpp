// Behavioral fixture for the production focus scanner; no Skyrim entry points.
#define TEXTBRIDGE_FOCUS_TEST
#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <utility>
namespace RE {
struct GFxValue {
    enum class Kind { None, String, Bool } kind{};
    std::string text; bool flag{};
    GFxValue() = default;
    GFxValue(const char* value) : kind(Kind::String), text(value) {}
    GFxValue(bool value) : kind(Kind::Bool), flag(value) {}
    bool IsString() const { return kind == Kind::String; }
    bool IsBool() const { return kind == Kind::Bool; }
    const char* GetString() const { return text.c_str(); }
    bool GetBool() const { return flag; }
};
struct Movie {
    std::unordered_map<std::string, GFxValue> values;
    bool selection = true, global = true;
    GFxValue focus;
    bool Invoke(const char* name, GFxValue* out, const void*, unsigned) {
        const bool supported = std::string_view(name) == "Selection.getFocus" ? selection : global;
        if (!supported) return false;
        *out = focus; return true;
    }
    bool GetVariable(GFxValue* out, const char* name) {
        const auto it = values.find(name); if (it == values.end()) return false;
        *out = it->second; return true;
    }
};
struct ControlMap {
    struct Data { int textEntryCount{}; } data;
    static ControlMap* instance;
    static ControlMap* GetSingleton() { return instance; }
    Data& GetRuntimeData() { return data; }
};
struct UI {
    std::unordered_map<std::string, Movie*> movies;
    static UI* instance;
    static UI* GetSingleton() { return instance; }
    bool IsMenuOpen(const char* name) { return movies.contains(name); }
    Movie* GetMovieView(const char* name) { return movies.at(name); }
};
ControlMap* ControlMap::instance{};
UI* UI::instance{};
}
#include "../src/SkyUiFocus.cpp"
int main() {
    unsigned checks{};
    auto check = [&](bool ok, const char* label) { if (!ok) throw std::runtime_error(label); ++checks; };
    RE::ControlMap controls; RE::UI ui; RE::Movie movie;
    RE::ControlMap::instance = &controls; RE::UI::instance = &ui;
    const std::string field = "_level0.Menu.textEntry.TextInputInstance.textField";
    ui.movies["RaceSex Menu"] = &movie;
    movie.focus = RE::GFxValue(field.c_str());
    movie.values[field + ".type"] = RE::GFxValue("input");
    check(TextBridge::SkyUiFocus::Target().empty(), "character customization alone does not enable input");
    controls.data.textEntryCount = 1;
    check(TextBridge::SkyUiFocus::Target() == "sky:RaceSex Menu:" + field, "naming field with text permission activates");
    movie.values["_level0.Menu.textEntry._visible"] = RE::GFxValue(false);
    check(TextBridge::SkyUiFocus::Target().empty(), "closed dialog with stale focus is rejected");
    movie.values["_level0.Menu.textEntry._visible"] = RE::GFxValue(true);
    check(!TextBridge::SkyUiFocus::Target().empty(), "reopened naming dialog resumes detection");
    movie.values[field + ".password"] = RE::GFxValue(true);
    check(TextBridge::SkyUiFocus::Target().empty(), "password field excluded");
    movie.values.erase(field + ".password");
    movie.values[field + ".type"] = RE::GFxValue("dynamic");
    check(TextBridge::SkyUiFocus::Target().empty(), "readonly text or slider label excluded");
    movie.values[field + ".type"] = RE::GFxValue("input");
    movie.values["_level0.Menu.textEntry.enabled"] = RE::GFxValue(false);
    check(TextBridge::SkyUiFocus::Target().empty(), "disabled containing control excluded");
    movie.values.erase("_level0.Menu.textEntry.enabled");
    movie.values[field + ".disabled"] = RE::GFxValue(true);
    check(TextBridge::SkyUiFocus::Target().empty(), "disabled focused field excluded");
    movie.values.erase(field + ".disabled");
    movie.selection = false;
    check(!TextBridge::SkyUiFocus::Target().empty(), "global Selection fallback works");
    movie.global = false;
    check(TextBridge::SkyUiFocus::Target().empty(), "missing focus API does not guess naming target");
    movie.global = true;
    movie.focus = RE::GFxValue("");
    check(TextBridge::SkyUiFocus::Target().empty(), "blur ends automatic target");
    movie.focus = RE::GFxValue(field.c_str());
    ui.movies.clear(); ui.movies["InventoryMenu"] = &movie;
    check(TextBridge::SkyUiFocus::Target() == "sky:InventoryMenu:" + field, "existing SkyUI focus remains supported");
    ui.movies.clear(); ui.movies["UnsupportedMenu"] = &movie;
    check(TextBridge::SkyUiFocus::Target().empty(), "unrecognized menus remain manual");
    ui.movies.clear(); ui.movies["RaceSex Menu"] = nullptr;
    check(TextBridge::SkyUiFocus::Target().empty(), "missing movie tolerated");
    RE::ControlMap::instance = nullptr;
    check(TextBridge::SkyUiFocus::Target().empty(), "missing engine state tolerated");
    std::cout << checks << " Scaleform focus checks passed (mock movies, production scanner)\n";
}
