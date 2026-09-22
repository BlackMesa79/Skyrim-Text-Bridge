#pragma once
#include <array>
#include <string>
namespace TextBridge::Settings {
struct Data {
    bool enabled = true;
    int hotkey = 66;
    float x = .025f, y = .065f, size = 1.f, opacity = .92f;
    std::array<float,3> background{25/255.f,28/255.f,32/255.f}, accent{212/255.f,190/255.f,139/255.f}, foreground{232/255.f,233/255.f,230/255.f};
};
Data Get();
void Set(Data value);
void Load(const std::wstring& path);
bool Save();
std::wstring KeyName(int scan);
}
namespace TextBridge::SettingsMenu {
void Install();
bool Blocking();
bool CaptureKey(unsigned scan);
}
