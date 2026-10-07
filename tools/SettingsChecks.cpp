#include "../src/Settings.cpp"
#include "PanelState.h"
#include <iostream>
#include <limits>
#include <filesystem>
int main() {
    using namespace TextBridge;
    auto path=std::filesystem::absolute("build/preview/settings-test.ini").wstring();
    Settings::Load(path); auto value=Settings::Data{};
    if (value.autoEnable) return 7;
    value.autoEnable=true; value.enabled=false; value.hotkey=65; value.x=.8f; value.y=.3f; value.opacity=.25f; value.background={.2f,.3f,.4f};
    Settings::Set(value); if (!Settings::Save()) return 1;
    Settings::Set({}); Settings::Load(path); auto loaded=Settings::Get();
    if (!loaded.autoEnable || loaded.enabled || loaded.hotkey!=65 || std::abs(loaded.opacity-.25f)>.0001f || loaded.background!=value.background) return 2;
    WritePrivateProfileStringW(L"Input",L"AutoEnable",nullptr,path.c_str());
    Settings::Load(path); if (Settings::Get().autoEnable) return 8;
    value.size=std::numeric_limits<float>::quiet_NaN(); value.opacity=2; value.x=-3;
    Settings::Set(value); auto clamped=Settings::Get(); if (clamped.size!=1 || clamped.opacity!=1 || clamped.x!=0) return 3;
    for (auto opacity : {0.f,.5f,1.f}) {
        value=Settings::Data{}; value.opacity=opacity; Settings::Set(value);
        if ((PanelColor(.2f,.3f,.4f,Settings::Get().opacity) >> 24) != unsigned(opacity*255+.5f)) return 4;
        if ((PanelColor(.9f,.9f,.9f) >> 24) != 255) return 5;
    }
    std::cout << "Settings persistence, invalid-value clamps, 0/50/100% ImGui background alpha and opaque text colors passed\n";
}
