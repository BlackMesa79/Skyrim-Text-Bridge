#include "DirectIme.h"
#include "Settings.h"
#include <cstdint>

// Optional native editor support. Unicode text continues through Skyrim CharEvents.
// No STL or ImGui context crosses the plugin boundary.
extern "C" __declspec(dllexport) std::uint32_t SkyrimTextBridge_QueryInputV1(std::uint32_t version,std::uint32_t* hotkey) {
    if(version!=1 || !hotkey)return 0;
    const auto settings=TextBridge::Settings::Get();
    *hotkey=static_cast<std::uint32_t>(settings.hotkey);
    return 1u | (settings.enabled?2u:0u) | (TextBridge::DirectIme::SuppressKeyboard()?4u:0u) |
        (TextBridge::DirectIme::BlockNavigation()?8u:0u);
}
extern "C" __declspec(dllexport) void SkyrimTextBridge_ResetInputV1() {
    TextBridge::DirectIme::Reset();
}
