#pragma once
#include <Windows.h>
#include <vector>
#include <cstdint>
namespace TextBridge::DirectIme {
bool Install(HWND window);
bool Toggle();
void Reset();
void Tick();
bool SuppressKeyboard();
bool Active();
bool BlockNavigation();
void ToggleEnglish();
std::vector<std::uint32_t> Take(unsigned capacity);
}
