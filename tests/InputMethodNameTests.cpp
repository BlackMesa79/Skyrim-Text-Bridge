#include "InputMethodName.h"
#include <iostream>
int main() {
    using namespace TextBridge::InputMethodName;
    if (Clean(L"  Microsoft Pinyin\r\n ") != L"Microsoft Pinyin") return 1;
    if (!Clean(L"\t\n").empty()) return 2;
    if (Clean(std::wstring(300,L'A')).size()!=160) return 3;
    if (Read().empty()) return 4;
    if (Label(L"微软拼音") != L"当前输入法：微软拼音") return 5;
    if (Label(L"中文（简体，中国）", true) != L"键盘语言：中文（简体，中国）") return 6;
    if (Label(L"  ", true) != L"当前输入法：未知") return 7;
    std::cout << "IME name sanitization, bounds and live Windows TSF/IMM/fallback query passed\n";
}
