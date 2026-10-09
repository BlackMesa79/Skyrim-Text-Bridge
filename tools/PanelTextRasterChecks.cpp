#include "PanelTextRaster.h"
#include <Windows.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>
int main() {
    using namespace TextBridge::PanelTextRaster;
    unsigned checks{};
    auto check = [&](bool ok, const char* name) { ++checks; if (!ok) { std::cerr << "FAIL " << name << '\n'; std::exit(1); } };
    check(!Render("bad", 0, 300, 32, 0xffffffff), "invalid size rejected");
    check(!Render("bad", 22, 3000, 32, 0xffffffff), "oversized bitmap rejected");
    check(!Render("\xf0\x80", 22, 300, 32, 0xffffffff), "invalid UTF8 rejected");
    const auto emoji = Render("😂", 30, 64, 44, 0xffffffff);
    check(bool(emoji), "real DirectWrite/WARP supplementary emoji render");
    unsigned visible{}, colored{};
    for (std::size_t i = 0; i < emoji.rgba.size(); i += 4) if (emoji.rgba[i + 3] > 100) {
        ++visible;
        if (std::max({emoji.rgba[i], emoji.rgba[i + 1], emoji.rgba[i + 2]}) -
            std::min({emoji.rgba[i], emoji.rgba[i + 1], emoji.rgba[i + 2]}) > 30) ++colored;
    }
    check(visible > 40 && colored > 20, "system color-font glyph has visible colored pixels");
    const auto question = Render("?", 30, 64, 44, 0xffffffff);
    check(question && question.rgba != emoji.rgba, "emoji was not replaced by question mark");
    const auto joined = Render("👩‍💻 ❤️ 🇨🇳 中文", 22, 280, 32, 0xffdadfe6);
    check(bool(joined), "ZWJ variation selector flag and Chinese fallback render together");
    const auto clipped = Render("很长的中文测试 😂😂😂😂😂", 22, 70, 32, 0xffffffff);
    check(clipped && clipped.width == 70 && clipped.height == 32, "bounded trimmed candidate raster");
    check(SavePng(emoji, L"build/preview/emoji-raster.png"), "WIC PNG output with alpha");
    check(SavePng(joined, L"build/preview/emoji-mixed-raster.png"), "mixed fallback preview output");
    std::cout << checks << " real Windows font/raster checks passed (no game texture upload)\n";
}
