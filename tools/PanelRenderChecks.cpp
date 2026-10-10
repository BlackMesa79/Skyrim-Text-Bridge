#define TEXTBRIDGE_PANEL_TEST
#include "../src/ImePanel.cpp"
#include "../src/Settings.cpp"
#include <iostream>
#include <stdexcept>
#include <thread>
namespace TextBridge::ImePanel {
namespace {
std::uint64_t panelClock = 100;
bool focused = true, apiPresent = true, hasContext = true, registerFails = false;
const char* missingExport{};
unsigned registrations{};
void(__stdcall* callback)(){};
ImGuiViewport view{};
struct TextCommand { std::string text; ImU32 color; ImVec4 clip; float size; ImVec2 position; };
std::vector<TextCommand> texts;
std::vector<ImU32> backgrounds;
std::int64_t Register(void(__stdcall* render)()) { if (registerFails) return -1; ++registrations; callback = render; return 0; }
ImGuiContext* Context() { return hasContext ? reinterpret_cast<ImGuiContext*>(1) : nullptr; }
ImGuiViewport* Viewport() { return &view; }
ImDrawList* DrawList() { return reinterpret_cast<ImDrawList*>(2); }
ImFont* Font() { return reinterpret_cast<ImFont*>(3); }
const ImFontGlyph* Glyph(ImFont*, ImWchar) { return reinterpret_cast<ImFontGlyph*>(4); }
void Rect(ImDrawList*, ImVec2, ImVec2, ImU32, float, ImDrawFlags, float) {}
void Fill(ImDrawList*, ImVec2, ImVec2, ImU32 color, float, ImDrawFlags) { backgrounds.push_back(color); }
void Text(ImDrawList*, const ImFont*, float size, ImVec2 position, ImU32 color, const char* start, const char* end, float, const ImVec4* clip) {
    texts.push_back({std::string(start, end), color, *clip, size, position});
}
void Measure(ImVec2* out, ImFont*, float size, float, float, const char* start, const char* end, const char**) {
    unsigned points{}; for (auto p = start; p != end; ++p) if ((static_cast<unsigned char>(*p) & 0xC0) != 0x80) ++points;
    *out = {points * size * .7f, size};
}
void* TestResolve(const char* name) {
    if (!apiPresent || (missingExport && std::string_view(name) == missingExport)) return nullptr;
#define EXPORT(exportName, function) if (std::string_view(name) == exportName) return reinterpret_cast<void*>(&function)
    EXPORT("RegisterHudElement", Register); EXPORT("igGetCurrentContext", Context);
    EXPORT("igGetMainViewport", Viewport); EXPORT("igGetForegroundDrawList_Nil", DrawList);
    EXPORT("igGetFont", Font); EXPORT("ImFont_FindGlyphNoFallback", Glyph);
    EXPORT("ImDrawList_AddRect", Rect); EXPORT("ImDrawList_AddRectFilled", Fill);
    EXPORT("ImDrawList_AddText_FontPtr", Text); EXPORT("ImFont_CalcTextSizeA", Measure);
#undef EXPORT
    return nullptr;
}
bool TestForeground(HWND) { return focused; }
std::uint64_t TestNow() { return panelClock; }
}
}
int main() {
    using namespace TextBridge;
    using namespace TextBridge::ImePanel;
    unsigned checks{};
    auto check = [&](bool ok, const char* label) { if (!ok) throw std::runtime_error(label); ++checks; };
    auto frame = [&] { texts.clear(); backgrounds.clear(); callback(); };
    apiPresent = false; check(!Install() && registrations == 0, "absent framework cannot register renderer");
    apiPresent = true;
    for (auto name : {"RegisterHudElement", "igGetCurrentContext", "igGetMainViewport", "igGetForegroundDrawList_Nil", "igGetFont",
                      "ImFont_FindGlyphNoFallback", "ImDrawList_AddRect", "ImDrawList_AddRectFilled", "ImDrawList_AddText_FontPtr", "ImFont_CalcTextSizeA"}) {
        missingExport = name; check(!Install() && registrations == 0, "incomplete backend rejected before registration");
    }
    missingExport = nullptr; registerFails = true; check(!Install(), "failed HUD registration rejected"); registerFails = false;
    check(Install() && Install() && registrations == 1, "HUD installs once; zero registration ID valid");
    view.Pos = {0, 0}; view.Size = {1920, 1080};
    frame(); check(texts.empty(), "idle creates no overlay");
    const auto owner = reinterpret_cast<HWND>(1);
    Show(owner, L"中文输入 · F8 关闭\nzhong wen\n当前输入法：微软拼音\n> 1. 中文##literal\n  2. 中问");
    frame(); check(!texts.empty() && !backgrounds.empty(), "snapshot draws without opening framework settings");
    check(std::any_of(texts.begin(), texts.end(), [](const auto& c) { return c.text == "中文##literal"; }), "candidate ## remains literal text");
    const auto candidate = std::find_if(texts.begin(), texts.end(), [](const auto& c) { return c.text == "中文##literal"; });
    check(candidate->size >= 22 && candidate->clip.w - candidate->clip.y == 28, "candidate font enlarged with compact row height");
    const auto next = std::find_if(texts.begin(), texts.end(), [](const auto& c) { return c.text == "中问"; });
    check(next->position.y - candidate->position.y == 28, "candidate baseline spacing reduced from 34 to 28");
    check(std::all_of(texts.begin(), texts.end(), [](const auto& c) { return (c.color >> 24) == 255; }), "text opacity independent of background");
    for (const auto alpha : {0.f, .5f, 1.f}) {
        auto value = Settings::Data{}; value.opacity = alpha; Settings::Set(value); frame();
        check((backgrounds.front() >> 24) == unsigned(alpha * 255.f + .5f), "background alpha honored at endpoints and midpoint");
    }
    auto value = Settings::Data{}; value.x = 1; value.y = 1; value.size = 1.8f; Settings::Set(value);
    view.Pos = {40, 30}; view.Size = {320, 200}; frame();
    check(std::all_of(texts.begin(), texts.end(), [](const auto& c) { return c.clip.x >= 40 && c.clip.y >= 30 && c.clip.z <= 360.01f && c.clip.w <= 230.01f; }), "small viewport and nonzero origin clamp all rows");
    view.Size = {0, 0}; frame(); check(texts.empty(), "minimized zero viewport skips drawing");
    view.Pos = {0, 0}; view.Size = {1920, 1080}; Settings::Set({});
    Show(owner, std::wstring(200, L'中')); frame();
    check(texts.size() >= 1 && texts.front().text.ends_with("…"), "long Chinese label clips on UTF-8 boundaries");
    Hide(); frame(); check(texts.empty(), "hide removes overlay immediately");
    Show(owner, L"输入法已关闭", true); panelClock += 1399; frame(); check(texts.size() == 1, "exit tip persists before deadline");
    ++panelClock; frame(); check(texts.empty(), "exit tip expires without a Windows timer");
    Show(owner, L"中文输入\n测试"); focused = false; frame(); focused = true; frame();
    check(texts.empty(), "focus loss removes stale display even after return");
    Show(owner, L"中文输入\n测试"); hasContext = false; frame(); check(texts.empty(), "framework not initialized safely skips drawing"); hasContext = true;
    Show(owner, L"中文输入\n测试");
    value = Settings::Data{}; value.enabled = false; Settings::Set(value); frame(); check(texts.empty(), "disabled mod does not render"); Settings::Set({});
    using Localization::Key;
    for (int locale=1;locale<=4;++locale) {
        value=Settings::Data{};value.language=locale;Settings::Set(value);
        const std::wstring content=std::wstring(Localization::Wide(Key::JapaneseInput))+L" · F8\n"+
            Localization::Wide(Key::Idle)+L"\n"+Localization::Wide(Key::ImeName)+L"Microsoft IME\n> 1. 日本語\n  2. 한국어";
        Show(owner,content);frame();
        check(std::any_of(texts.begin(),texts.end(),[&](const auto& cmd){return cmd.text==Localization::Text(Key::JapaneseInput);}),
              "panel title uses chosen locale");
        check(std::any_of(texts.begin(),texts.end(),[&](const auto& cmd){return cmd.text==Localization::Text(Key::Ready);}),
              "translated idle status receives ready style");
        check(std::any_of(texts.begin(),texts.end(),[](const auto& cmd){return cmd.text=="日本語";}) &&
              std::any_of(texts.begin(),texts.end(),[](const auto& cmd){return cmd.text=="한국어";}),
              "Japanese and Hangul candidates preserve exact Unicode across interface locales");
        Show(owner,Localization::Wide(Key::Disabled),true);frame();
        check(texts.size()==1 && texts.front().text==Localization::Text(Key::Disabled), "translated transient exit notice");
    }
    Settings::Set({});
    PanelState shared;
    std::thread writer([&] { for (unsigned i = 0; i < 10000; ++i) { shared.Show(1, "title\nChinese\ncandidate", false, i); if (i % 3 == 0) shared.Hide(); } });
    bool whole = true;
    for (unsigned i = 0; i < 10000; ++i) { const auto snapshot = shared.Read(); whole &= snapshot.lines.empty() || (snapshot.lines.size() == 3 && snapshot.lines[0] == "title" && snapshot.lines[2] == "candidate"); }
    writer.join(); check(whole, "concurrent publication never exposes partial candidate snapshots");
    std::cout << checks << " ImGui panel lifecycle/render checks passed (mock public API)\n";
}
