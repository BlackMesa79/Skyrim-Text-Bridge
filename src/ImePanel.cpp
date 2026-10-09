#ifndef TEXTBRIDGE_PANEL_TEST
#include <SKSE/SKSE.h>
#endif
namespace RE { class InputEvent; }
#include "ImePanel.h"
#include "Settings.h"
#include "PanelState.h"
#include "PanelTextRaster.h"
#include <atomic>
#include <filesystem>
#include <limits>
#include <unordered_map>
#include <cmath>
#include "../extern/SKSEMenuFramework/SKSEMenuFramework.h"

namespace TextBridge::ImePanel {
namespace {
PanelState state;
bool installed = false;
bool warnedFont = false;
using namespace ImGuiMCP;
struct Backend {
    std::int64_t (*registerHud)(void(__stdcall*)()){};
    ImGuiContext* (*context)(){};
    ImGuiViewport* (*viewport)(){};
    ImDrawList* (*drawList)(){};
    ImFont* (*font)(){};
    const ImFontGlyph* (*glyph)(ImFont*, ImWchar){};
    void (*rect)(ImDrawList*, ImVec2, ImVec2, ImU32, float, ImDrawFlags, float){};
    void (*fill)(ImDrawList*, ImVec2, ImVec2, ImU32, float, ImDrawFlags){};
    void (*text)(ImDrawList*, const ImFont*, float, ImVec2, ImU32, const char*, const char*, float, const ImVec4*){};
    void (*measure)(ImVec2*, ImFont*, float, float, float, const char*, const char*, const char**){};
    ImTextureID (*loadTexture)(const char*, ImVec2*){};
    void (*disposeTexture)(const char*){};
    void (*image)(ImDrawList*, ImTextureID, ImVec2, ImVec2, ImVec2, ImVec2, ImU32){};
} backend;
struct CachedText { ImTextureID texture{}; std::string path; std::uint64_t used{}; };
std::unordered_map<std::string, CachedText> rasterCache;
std::uint64_t renderFrame{};
unsigned rasterBudget{};
bool warnedRaster{};
#ifdef TEXTBRIDGE_PANEL_TEST
extern void* TestResolve(const char*);
extern bool TestForeground(HWND);
extern std::uint64_t TestNow();
void* Resolve(const char* name) { return TestResolve(name); }
bool Foreground(HWND owner) { return TestForeground(owner); }
std::uint64_t Now() { return TestNow(); }
void Warn(const char*) {}
#else
void* Resolve(const char* name) {
    const auto module = GetModuleHandleW(L"SKSEMenuFramework.dll");
    return module ? reinterpret_cast<void*>(GetProcAddress(module, name)) : nullptr;
}
bool Foreground(HWND owner) { return IsWindow(owner) && GetForegroundWindow() == owner && !IsIconic(owner); }
std::uint64_t Now() { return GetTickCount64(); }
void Warn(const char* message) { SKSE::log::warn("{}", message); }
#endif
template <typename T> bool Bind(T& function, const char* name) {
    function = reinterpret_cast<T>(Resolve(name)); return function != nullptr;
}
std::string Utf8(const std::wstring& text) {
    const int length = static_cast<int>(std::min<std::size_t>(text.size(), 16384));
    const auto size = WideCharToMultiByte(CP_UTF8, 0, text.data(), length, nullptr, 0, nullptr, nullptr);
    std::string result(size, 0);
    if (size) WideCharToMultiByte(CP_UTF8, 0, text.data(), length, result.data(), size, nullptr, nullptr);
    return result;
}
bool NeedsFallback(ImFont* font, std::string_view text) {
    for (std::size_t i = 0; i < text.size();) {
        auto value = static_cast<unsigned char>(text[i++]);
        std::uint32_t point = value;
        unsigned more = 0;
        if (value >= 0xf0) { point = value & 7; more = 3; }
        else if (value >= 0xe0) { point = value & 15; more = 2; }
        else if (value >= 0xc0) { point = value & 31; more = 1; }
        for (unsigned j = 0; j < more && i < text.size(); ++j) point = (point << 6) | (static_cast<unsigned char>(text[i++]) & 63);
        if (point > 0xffff || !backend.glyph(font, static_cast<ImWchar>(point))) return true;
    }
    return false;
}
void TrimRasterCache(bool visible) {
    ++renderFrame; rasterBudget = 2;
    for (auto it = rasterCache.begin(); it != rasterCache.end();) {
        if (renderFrame - it->second.used > (visible ? 120u : 3u)) {
            if (it->second.texture) backend.disposeTexture(it->second.path.c_str());
            it = rasterCache.erase(it);
        } else ++it;
    }
}
ImTextureID RasterLabel(const std::string& text, float size, float width, float height, ImU32 color) {
    if (!backend.loadTexture || !backend.disposeTexture || !backend.image) return {};
    const auto w = static_cast<unsigned>(std::ceil(width)), h = static_cast<unsigned>(std::ceil(height));
    const auto roundedSize = std::round(size * 4) / 4;
    const auto key = text + '\0' + std::to_string(roundedSize) + ':' + std::to_string(w) + ':' + std::to_string(h) + ':' + std::to_string(color);
    if (auto found = rasterCache.find(key); found != rasterCache.end()) { found->second.used = renderFrame; return found->second.texture; }
    if (!rasterBudget) return {};
    if (rasterCache.size() >= 32) {
        auto oldest = std::min_element(rasterCache.begin(), rasterCache.end(), [](const auto& a, const auto& b) { return a.second.used < b.second.used; });
        if (renderFrame - oldest->second.used <= 3) return {};
        backend.disposeTexture(oldest->second.path.c_str()); rasterCache.erase(oldest);
    }
    --rasterBudget;
    const auto bitmap = PanelTextRaster::Render(text, roundedSize, w, h, color);
    if (!bitmap) return {};
    wchar_t directory[MAX_PATH]{}, filename[MAX_PATH]{};
    if (!GetTempPathW(MAX_PATH, directory) || !GetTempFileNameW(directory, L"STB", 0, filename)) return {};
    const auto path = Utf8(filename);
    ImTextureID texture{};
    if (PanelTextRaster::SavePng(bitmap, filename)) texture = backend.loadTexture(path.c_str(), nullptr);
    // The public loader caches the texture. Never retain candidate text on disk
    // or include text in filenames; its process-private key is the random path.
    DeleteFileW(filename);
    if (texture) rasterCache.emplace(key, CachedText{texture, path, renderFrame});
    else if (!warnedRaster) { warnedRaster = true; Warn("Text Bridge system-font fallback texture unavailable"); }
    return texture;
}
void __stdcall Render() {
    const auto panel = state.Read();
    TrimRasterCache(panel.Visible(Now()));
    if (!panel.Visible(Now())) return;
    if (!Foreground(reinterpret_cast<HWND>(panel.owner))) { state.Hide(); return; }
    const auto settings = Settings::Get();
    if (!settings.enabled || !backend.context()) return;
    const auto viewport = backend.viewport();
    const auto draw = backend.drawList();
    const auto font = backend.font();
    if (!viewport || !draw || !font) return;
    if (!warnedFont && !backend.glyph(font, 0x4E2D)) {
        warnedFont = true;
        Warn("Text Bridge panel: framework font lacks Chinese glyphs; configure EnableChinese=true and a Chinese PrimaryFont in SKSEMenuFramework.ini, then restart");
    }
    const auto box = FitPanel(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y,
                              settings.size, settings.x, settings.y, panel);
    if (box.scale <= 0) return;
    const auto color = [](const std::array<float, 3>& c, float alpha = 1.f) { return PanelColor(c[0], c[1], c[2], alpha); };
    const auto foreground = color(settings.foreground), accent = color(settings.accent);
    const auto muted = PanelColor(.58f, .60f, .62f);
    auto point = [&](float x, float y) { return ImVec2{box.x + x * box.scale, box.y + y * box.scale}; };
    auto fill = [&](float x, float y, float right, float bottom, ImU32 c, float radius = 0.f) {
        backend.fill(draw, point(x, y), point(right, bottom), c, radius * box.scale, 0);
    };
    auto label = [&](std::string text, float x, float y, float right, float bottom, float size, ImU32 c, bool center = false) {
        const float actualSize = size * box.scale, available = (right - x) * box.scale;
        if (NeedsFallback(font, text)) {
            if (const auto texture = RasterLabel(text, actualSize, available, (bottom - y) * box.scale, c)) {
                backend.image(draw, texture, point(x, y), point(right, bottom), {0, 0}, {1, 1}, 0xffffffff);
                return;
            }
        }
        auto widthOf = [&](const std::string& value) {
            ImVec2 measured{};
            backend.measure(&measured, font, actualSize, std::numeric_limits<float>::max(), 0, value.data(), value.data() + value.size(), nullptr);
            return measured.x;
        };
        float width = widthOf(text);
        if (width > available) {
            const float ellipsis = widthOf("…");
            do {
                if (text.empty()) break;
                auto end = text.size() - 1;
                while (end && (static_cast<unsigned char>(text[end]) & 0xC0) == 0x80) --end;
                text.resize(end); width = widthOf(text);
            } while (width + ellipsis > available);
            text += "…"; width += ellipsis;
        }
        const auto pos = point(x, y + (bottom - y - size) * .5f);
        const ImVec4 clip{box.x + x * box.scale, box.y + y * box.scale, box.x + right * box.scale, box.y + bottom * box.scale};
        backend.text(draw, font, actualSize, {pos.x + (center ? std::max(0.f, (available - width) * .5f) : 0.f), pos.y},
                     c, text.data(), text.data() + text.size(), 0, &clip);
    };
    const float height = box.height / box.scale;
    fill(0, 0, 380, height, color(settings.background, settings.opacity), 8);
    backend.rect(draw, point(0, 0), point(380, height), PanelColor(.28f, .29f, .30f, settings.opacity), 8 * box.scale, 0, box.scale);
    std::string title = panel.lines.front();
    if (const auto separator = title.find(" ·"); separator != std::string::npos) title.resize(separator);
    fill(18, 21, 23, 26, panel.transient ? muted : accent);
    label(title, 33, 8, panel.transient ? 362 : 280, 40, 18, foreground);
    if (panel.transient) return;
    fill(298, 12, 362, 36, PanelColor(.17f, .18f, .20f, settings.opacity), 4);
    label(Utf8(Settings::KeyName(settings.hotkey)) + " 关闭", 282, 12, 362, 36, 13, muted, true);
    fill(18, 44, 362, 45, PanelColor(.20f, .22f, .23f, settings.opacity));
    if (panel.lines.size() > 1) {
        const bool idle = panel.lines[1] == "请在当前输入框打字";
        label(idle ? "就绪 · 在当前输入框开始输入" : panel.lines[1], 20, 48, 360, 82, idle ? 17.f : 24.f, idle ? muted : accent);
    }
    for (std::size_t i = 2; i < panel.lines.size(); ++i) {
        auto line = panel.lines[i];
        const bool selected = line.starts_with("> ");
        const float y = 86 + float(i - 2) * 28;
        if (selected) {
            fill(12, y, 368, y + 28, PanelColor(.22f, .21f, .18f, settings.opacity), 4);
            fill(12, y + 4, 15, y + 24, accent);
        }
        if (const auto first = line.find_first_not_of(" >"); first != std::string::npos) line.erase(0, first);
        const auto dot = line.find(". ");
        if (dot != std::string::npos && dot <= 2) {
            label(line.substr(0, dot), 24, y, 49, y + 28, 15, selected ? accent : muted);
            label(line.substr(dot + 2), 57, y, 356, y + 28, 22, selected ? accent : foreground);
        } else label(line, 20, y, 360, y + 28, 14, muted);
    }
}
}
bool Install() {
    if (installed) return true;
    Backend next;
    const bool complete = Bind(next.registerHud, "RegisterHudElement") && Bind(next.context, "igGetCurrentContext") &&
        Bind(next.viewport, "igGetMainViewport") && Bind(next.drawList, "igGetForegroundDrawList_Nil") &&
        Bind(next.font, "igGetFont") && Bind(next.glyph, "ImFont_FindGlyphNoFallback") &&
        Bind(next.rect, "ImDrawList_AddRect") && Bind(next.fill, "ImDrawList_AddRectFilled") &&
        Bind(next.text, "ImDrawList_AddText_FontPtr") && Bind(next.measure, "ImFont_CalcTextSizeA");
    if (!complete) { Warn("Text Bridge requires SKSE Menu Framework with HUD/ImGui exports for its in-game panel; input hook remains disabled"); return false; }
    backend = next;
    // Optional supplementary glyph support through public texture exports.
    Bind(backend.loadTexture, "LoadTexture"); Bind(backend.disposeTexture, "DisposeTexture");
    Bind(backend.image, "ImDrawList_AddImage");
    if (backend.registerHud(Render) < 0) { backend = {}; Warn("Text Bridge HUD registration failed"); return false; }
    installed = true;
    return true;
}
void Show(HWND owner, const std::wstring& text, bool transient) { state.Show(reinterpret_cast<std::uintptr_t>(owner), Utf8(text), transient, Now()); }
void Hide() { state.Hide(); }
}
