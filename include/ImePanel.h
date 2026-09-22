#pragma once
#include <Windows.h>
#include <string>
namespace TextBridge::ImePanel {
void Show(HWND owner, const std::wstring& text, bool transient = false);
void Hide();
}
