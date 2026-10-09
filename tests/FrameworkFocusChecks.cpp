// Production scanner/router fixtures; no Skyrim entry points or ImGui ABI reads.
#define TEXTBRIDGE_FOCUS_TEST
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <functional>
namespace RE {
struct Lock { unsigned held{}; };
struct BSSpinLockGuard { Lock& lock; BSSpinLockGuard(Lock& v) : lock(v) { ++lock.held; } ~BSSpinLockGuard() { --lock.held; } };
template<class T> using GPtr = std::shared_ptr<T>;
struct GFxValue {
    enum class Kind { None, String, Bool } kind{};
    std::string text; bool flag{};
    GFxValue() = default;
    GFxValue(const char* v) : kind(Kind::String), text(v) {}
    GFxValue(bool v) : kind(Kind::Bool), flag(v) {}
    bool IsString() const { return kind == Kind::String; }
    bool IsBool() const { return kind == Kind::Bool; }
    const char* GetString() const { return text.c_str(); }
    bool GetBool() const { return flag; }
};
struct GFxMovieView {
    std::unordered_map<std::string, GFxValue> values;
    bool selection = true, global = true, visible = true;
    GFxValue focus;
    Lock* lock{}; bool* closeOnInvoke{}; unsigned queries{};
    std::function<void()> onQuery;
    bool GetVisible() const { return visible; }
    bool Invoke(const char* name, GFxValue* out, const void*, unsigned) {
        if (lock && lock->held) throw std::runtime_error("ActionScript invoked under engine lock");
        if (closeOnInvoke) *closeOnInvoke = false;
        if (onQuery) { auto call = std::exchange(onQuery, {}); call(); }
        ++queries;
        if (!(std::string_view(name) == "Selection.getFocus" ? selection : global)) return false;
        *out = focus; return true;
    }
    bool GetVariable(GFxValue* out, const char* name) {
        const auto it = values.find(name); if (it == values.end()) return false;
        *out = it->second; return true;
    }
};
struct IMenu {
    enum class Context { kNone, kGameplay, kCursor, kDebugOverlay, kMenuMode, kItemMenu };
    struct Input { Context value = Context::kMenuMode; Context get() const { return value; } } inputContext;
    bool onStack = true, usesMenu = true, modal = false, custom = false;
    bool pauses = false, inventory = false, application = false, always = false, assignCursor = false, saving = false;
    int depthPriority = 3;
    GPtr<GFxMovieView> uiMovie;
    bool OnStack() const { return onStack; }
    bool UsesMenuContext() const { return usesMenu; }
    bool Modal() const { return modal; }
    bool CustomRendering() const { return custom; }
    bool PausesGame() const { return pauses; }
    bool InventoryItemMenu() const { return inventory; }
    bool ApplicationMenu() const { return application; }
    bool AlwaysOpen() const { return always; }
    bool AssignCursorToRenderer() const { return assignCursor; }
    bool AllowSaving() const { return saving; }
};
struct ControlMap {
    struct Data { int textEntryCount{}; } data;
    static ControlMap* instance;
    static ControlMap* GetSingleton() { return instance; }
    Data& GetRuntimeData() { return data; }
};
struct Name { std::string value; const char* c_str() const { return value.c_str(); } };
struct UI {
    struct Entry { GPtr<IMenu> menu; };
    std::vector<std::pair<Name, Entry>> menuMap;
    std::vector<GPtr<IMenu>> menuStack;
    Lock processMessagesLock;
    bool visible = true, closingAllMenus = false;
    static UI* instance;
    static UI* GetSingleton() { return instance; }
    bool IsShowingMenus() const { return visible; }
    void Add(std::string name, GPtr<IMenu> menu) { menuMap.push_back({{std::move(name)}, {menu}}); menuStack.push_back(menu); }
    void Clear() { menuMap.clear(); menuStack.clear(); }
};
ControlMap* ControlMap::instance{};
UI* UI::instance{};
}
namespace { bool prisma{}, web{}; std::string webField; unsigned webStarts{}; bool probeEnabled{}; }
namespace TextBridge::Prisma { bool OwnsInput() { return prisma; } }
namespace TextBridge::Meridian {
bool HasFocus() { return web; }
std::string AutoTarget() { return webField; }
void PollAutoFocus(bool enabled) { probeEnabled = enabled; }
bool Begin(std::string_view id) { ++webStarts; return web && id == webField; }
}
#include "../src/ScaleformFocus.cpp"
#include "../src/FrameworkFocus.cpp"
#include "AutoInputPolicy.h"
int main() {
    unsigned checks{};
    auto check = [&](bool ok, const char* label) { if (!ok) throw std::runtime_error(label); ++checks; };
    using namespace TextBridge;
    using Backend = FrameworkFocus::Backend;
    RE::ControlMap controls; RE::UI ui;
    auto movie = std::make_shared<RE::GFxMovieView>();
    auto menu = std::make_shared<RE::IMenu>(); menu->uiMovie = movie; movie->lock = &ui.processMessagesLock;
    RE::ControlMap::instance = &controls; RE::UI::instance = &ui;
    const std::string field = "_level0.Menu.textEntry.TextInputInstance.textField";
    ui.Add("RaceSex Menu", menu);
    movie->focus = RE::GFxValue(field.c_str()); movie->values[field + ".type"] = RE::GFxValue("input");
    check(ScaleformFocus::Target().empty(), "character customization without permission never starts typing");
    controls.data.textEntryCount = 1;
    const auto naming = ScaleformFocus::Target();
    check(naming.starts_with("sf:") && naming.find("RaceSex Menu") != std::string::npos && naming.ends_with(field), "RaceMenu naming remains detectable");
    check(!ui.processMessagesLock.held, "engine lock released before ActionScript");
    movie->values["_level0.Menu.textEntry._visible"] = RE::GFxValue(false);
    check(ScaleformFocus::Target().empty(), "closed dialog with stale focus rejected");
    movie->values["_level0.Menu.textEntry._visible"] = RE::GFxValue(true);
    check(ScaleformFocus::Target() == naming, "reopened naming dialog detectable");
    for (const auto& property : {field + ".password", field + ".readOnly", field + ".disabled", std::string("_level0.Menu.password")}) {
        movie->values[property] = RE::GFxValue(true);
        check(ScaleformFocus::Target().empty(), "positive readonly/disabled/password rejects field or ancestor");
        movie->values.erase(property);
    }
    for (const auto& property : {field + ".editable", field + ".enabled", std::string("_level0.Menu.enabled")}) {
        movie->values[property] = RE::GFxValue(false);
        check(ScaleformFocus::Target().empty(), "negative editable/enabled rejects field or ancestor");
        movie->values.erase(property);
    }
    movie->values[field + ".type"] = RE::GFxValue("dynamic");
    check(ScaleformFocus::Target().empty(), "readonly labels/sliders never activate");
    movie->values[field + ".type"] = RE::GFxValue("input"); movie->selection = false;
    check(ScaleformFocus::Target() == naming, "global Selection fallback works");
    movie->global = false;
    check(ScaleformFocus::Target().empty(), "missing focus API never guessed");
    movie->global = true; movie->focus = RE::GFxValue("");
    check(ScaleformFocus::Target().empty(), "blur ends target");
    movie->focus = RE::GFxValue(field.c_str()); ui.Clear(); ui.Add("InventoryMenu", menu);
    check(ScaleformFocus::Target().find("InventoryMenu") != std::string::npos, "SkyUI remains supported");
    // Real native flags/depths from the pinned CommonLib menu headers, rather
    // than assuming every Scaleform owner has a menu context and no custom draw.
    for (const auto* name : {"InventoryMenu", "MagicMenu", "ContainerMenu", "BarterMenu"}) {
        ui.Clear();
        auto item = std::make_shared<RE::IMenu>(); item->uiMovie = movie;
        item->custom = item->pauses = item->inventory = true; item->depthPriority = 0;
        item->usesMenu = std::string_view(name) != "InventoryMenu";
        item->inputContext.value = item->usesMenu ? RE::IMenu::Context::kItemMenu : RE::IMenu::Context::kNone;
        ui.Add(name, item);
        auto detected = ScaleformFocus::Target();
        check(detected.find(name) != std::string::npos, "native item menu CustomRendering preserves Scaleform capability");
        auto cursor = std::make_shared<RE::IMenu>();
        cursor->usesMenu = false; cursor->inputContext.value = RE::IMenu::Context::kNone;
        cursor->custom = cursor->saving = true; cursor->depthPriority = 13; ui.Add("Cursor Menu", cursor);
        check(ScaleformFocus::Target() == detected, "native cursor depth 13 custom draw with no movie stays passive");
        auto hud = std::make_shared<RE::IMenu>(*cursor);
        hud->always = hud->assignCursor = true; hud->depthPriority = 2; ui.Add("HUD Menu", hud);
        auto fader = std::make_shared<RE::IMenu>(*cursor);
        fader->always = true; fader->depthPriority = 3; ui.Add("Fader Menu", fader);
        check(ScaleformFocus::Target() == detected, "native HUD and fader don't mask depth zero item fields");
        check(FrameworkFocus::Begin(FrameworkFocus::Current()), "native stack passes activation recheck");
        auto blocker = std::make_shared<RE::IMenu>(*cursor);
        blocker->modal = true; blocker->depthPriority = 14; ui.Add("OtherAuthor.Dialog", blocker);
        check(ScaleformFocus::Target().empty(), "modal role overrides passive-looking render flags");
        blocker->modal = false; blocker->pauses = true; blocker->always = true;
        check(ScaleformFocus::Target().empty(), "pausing loading-style layer blocks stale focus");
        blocker->pauses = false; blocker->application = true;
        check(ScaleformFocus::Target().empty(), "application ownership overrides passive-looking flags");
        blocker->onStack = false;
        movie->focus = RE::GFxValue("");
        check(ScaleformFocus::Target().empty(), "native custom-rendered menu without field stays inactive");
        movie->focus = RE::GFxValue(field.c_str());
        movie->onQuery = [&] { blocker->onStack = true; };
        check(ScaleformFocus::Target().empty(), "new owner during native stack query invalidates focus");
    }
    ui.Clear(); ui.Add("AnotherAuthor.CustomEditor", menu);
    const auto custom = ScaleformFocus::Target();
    check(custom.find("AnotherAuthor.CustomEditor") != std::string::npos, "unknown custom Scaleform menu needs no whitelist");
    movie->visible = false; check(ScaleformFocus::Target().empty(), "hidden movie rejected");
    movie->visible = true; ui.visible = false; check(ScaleformFocus::Target().empty(), "hidden menus never activate");
    ui.visible = true; ui.closingAllMenus = true; check(ScaleformFocus::Target().empty(), "closing-all transition rejected");
    ui.closingAllMenus = false; menu->onStack = false; check(ScaleformFocus::Target().empty(), "cached closed menu ignored");
    menu->onStack = true;
    auto overlay = std::make_shared<RE::IMenu>(); overlay->depthPriority = 10; ui.Add("OverlayDialog", overlay);
    check(ScaleformFocus::Target().empty(), "top owner without movie blocks underlying stale field");
    overlay->usesMenu = false; overlay->inputContext.value = RE::IMenu::Context::kCursor;
    check(ScaleformFocus::Target() == custom, "cursor overlay doesn't own text input");
    overlay->modal = true; check(ScaleformFocus::Target().empty(), "modal overlay blocks despite cursor context");
    overlay->modal = false; overlay->custom = true; check(ScaleformFocus::Target().empty(), "custom renderer blocks underlying auto input");
    overlay->custom = false; overlay->usesMenu = true;
    overlay->uiMovie = std::make_shared<RE::GFxMovieView>(*movie);
    overlay->uiMovie->focus = RE::GFxValue("_root.search"); overlay->uiMovie->values["_root.search.type"] = RE::GFxValue("input");
    check(ScaleformFocus::Target().ends_with("_root.search"), "top dialog editable field wins");
    overlay->depthPriority = menu->depthPriority;
    check(ScaleformFocus::Target().find("OverlayDialog") != std::string::npos, "equal depth uses latest stack owner");
    overlay->onStack = false; check(ScaleformFocus::Target() == custom, "closing overlay resumes underlying field");
    movie->focus = RE::GFxValue(std::string(513,'a').c_str()); check(ScaleformFocus::Target().empty(), "oversized paths rejected");
    movie->focus = RE::GFxValue("_root.bad\npath"); check(ScaleformFocus::Target().empty(), "control characters in path rejected");
    movie->focus = RE::GFxValue(field.c_str()); movie->closeOnInvoke = &menu->onStack;
    check(ScaleformFocus::Target().empty(), "menu closing during focus query cannot activate");
    movie->closeOnInvoke = nullptr; menu->onStack = true;
    movie->onQuery = [&] { controls.data.textEntryCount = 0; };
    check(ScaleformFocus::Target().empty(), "permission lost inside focus query invalidates activation");
    controls.data.textEntryCount = 1;
    movie->onQuery = [&] { overlay->onStack = true; };
    check(ScaleformFocus::Target().empty(), "new top dialog during query cannot activate old field");
    overlay->onStack = false;
    auto duringQuery = std::make_shared<RE::GFxMovieView>(*movie);
    movie->onQuery = [&] { menu->uiMovie = duringQuery; };
    check(ScaleformFocus::Target().empty(), "movie replaced during query cannot activate retained old movie");
    menu->uiMovie = movie;
    movie->onQuery = [&] { ui.visible = false; };
    check(ScaleformFocus::Target().empty(), "UI hidden during query invalidates activation");
    ui.visible = true;
    auto target = FrameworkFocus::Current();
    check(target.backend == Backend::Scaleform && target.identity == custom && FrameworkFocus::Begin(target), "router starts generic Scaleform field");
    const auto queries = movie->queries; prisma = true;
    check(ScaleformFocus::Target().empty() && movie->queries == queries, "Prisma blocks probing underlying movies");
    check(FrameworkFocus::Current().backend == Backend::Native && FrameworkFocus::Current().identity.empty() && !FrameworkFocus::Begin(target), "native backend wins and declines activation");
    FrameworkFocus::Poll(true); check(!probeEnabled, "Prisma stops web observer");
    prisma = false; web = true; webField = "mer:8:1:2";
    auto mer = FrameworkFocus::Current();
    check(mer.backend == Backend::Meridian && mer.identity == webField && FrameworkFocus::Begin(mer) && webStarts == 1, "Meridian uses existing capture API");
    webField.clear();
    check(FrameworkFocus::Current().backend == Backend::Meridian && FrameworkFocus::Current().identity.empty(), "web view without editable field never falls through");
    check(!FrameworkFocus::Begin(mer), "changed web field rejects late capture");
    FrameworkFocus::Poll(true); check(probeEnabled, "permitted observer runs");
    FrameworkFocus::Poll(false); check(!probeEnabled, "manual-only mode stops observer"); web = false;
    movie->focus = RE::GFxValue("_root.changed"); movie->values["_root.changed.type"] = RE::GFxValue("input");
    check(!FrameworkFocus::Begin(target), "changed field rejects late activation"); movie->focus = RE::GFxValue(field.c_str());
    auto replacement = std::make_shared<RE::GFxMovieView>(*movie); menu->uiMovie = replacement;
    check(ScaleformFocus::Target() != custom && !FrameworkFocus::Begin(target), "recreated movie cannot inherit old identity"); menu->uiMovie = movie;
    AutoInputPolicy policy; using Action = AutoInputPolicy::Action;
    check(policy.Update(custom,true,false,false) == Action::Start, "generic field starts automatic policy");
    policy.Started(custom); policy.Manual(custom,false);
    check(policy.Update(custom,true,false,false) == Action::None, "manual off suppresses same generic field");
    policy.Update("",true,false,false);
    check(policy.Update(custom,true,false,false) == Action::Start, "blur and reentry rearms field"); policy.Started(custom);
    check(policy.Update("",true,true,false) == Action::Stop, "focus loss stops owned session");
    menu->uiMovie.reset(); check(ScaleformFocus::Target().empty(), "missing movie tolerated");
    ui.Clear(); check(FrameworkFocus::Current().backend == Backend::None, "empty stack stays manual");
    RE::ControlMap::instance = nullptr; check(ScaleformFocus::Target().empty(), "missing engine state tolerated");
    RE::ControlMap::instance = &controls; RE::UI::instance = nullptr; check(ScaleformFocus::Target().empty(), "missing UI tolerated");
    std::cout << checks << " Scaleform/framework focus checks passed (production scanner/router, mock engine/movies)\n";
}
