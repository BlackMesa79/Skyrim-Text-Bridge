#include "InputMethodName.h"
#include <iostream>
int main() {
    using namespace TextBridge::InputMethodName;
    if (Clean(L"  Microsoft Pinyin\r\n ") != L"Microsoft Pinyin") return 1;
    if (!Clean(L"\t\n").empty()) return 2;
    if (Clean(std::wstring(300,L'A')).size()!=160) return 3;
    if (Read().empty()) return 4;
    std::cout << "IME name sanitization, bounds and live Windows TSF/IMM/fallback query passed\n";
}
