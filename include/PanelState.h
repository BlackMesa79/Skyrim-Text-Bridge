#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>
namespace TextBridge {
struct PanelSnapshot {
    std::uintptr_t owner{};
    std::vector<std::string> lines;
    bool transient = false;
    std::uint64_t expires{};
    bool Visible(std::uint64_t now) const { return !lines.empty() && (!transient || now < expires); }
};
// Publish whole snapshots between the IME/window and render threads. No IMM
// context or ImGui object may be shared across these threads.
class PanelState {
    std::mutex mutex;
    PanelSnapshot current;
public:
    void Show(std::uintptr_t owner, std::string text, bool transient, std::uint64_t now) {
        PanelSnapshot next{owner, {}, transient, transient ? now + 1400 : 0};
        std::size_t start = 0;
        do {
            const auto end = text.find('\n', start);
            next.lines.push_back(text.substr(start, end - start));
            if (end == std::string::npos || transient || next.lines.size() == 16) break;
            start = end + 1;
        } while (start < text.size());
        if (text.empty()) next.lines.clear();
        std::lock_guard lock(mutex); current = std::move(next);
    }
    void Hide() { std::lock_guard lock(mutex); current = {}; }
    PanelSnapshot Read() { std::lock_guard lock(mutex); return current; }
};
struct PanelLayout { float x{}, y{}, width{}, height{}, scale{}; };
inline PanelLayout FitPanel(float left, float top, float width, float height, float size, float x, float y,
                            const PanelSnapshot& panel) {
    if (!(width > 0 && height > 0) || !std::isfinite(width) || !std::isfinite(height)) return {};
    const float baseHeight = panel.transient ? 52.f : 94.f + float(panel.lines.size() > 2 ? panel.lines.size() - 2 : 0) * 28.f;
    const float scale = std::min({std::clamp(height / 1080.f, .75f, 2.5f) * size, width / 380.f, height / baseHeight});
    return {left + (width - 380.f * scale) * x, top + (height - baseHeight * scale) * y,
            380.f * scale, baseHeight * scale, scale};
}
inline std::uint32_t PanelColor(float r, float g, float b, float alpha = 1) {
    auto byte = [](float value) { return std::uint32_t(std::clamp(value, 0.f, 1.f) * 255.f + .5f); };
    return byte(r) | (byte(g) << 8) | (byte(b) << 16) | (byte(alpha) << 24);
}
}
