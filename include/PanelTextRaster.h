#pragma once
#include <cstdint>
#include <string_view>
#include <vector>
namespace TextBridge::PanelTextRaster {
struct Bitmap {
    unsigned width{}, height{};
    std::vector<std::uint8_t> rgba;
    explicit operator bool() const { return width && height && rgba.size() == width * height * 4; }
};
// Own offscreen WARP/DirectWrite renderer. Never touches the game's device,
// foreign ImGui font atlas, input context, or selected input-method profile.
Bitmap Render(std::string_view text, float size, unsigned width, unsigned height, std::uint32_t color);
bool SavePng(const Bitmap& image, const wchar_t* path);
#ifdef TEXTBRIDGE_RASTER_TEST
unsigned TestInitializationCount();
#endif
}
