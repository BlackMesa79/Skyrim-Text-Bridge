#pragma once
#include <string_view>
namespace TextBridge::ModexMenuGuard {
void Refresh();
void ObserveEscape(bool up, bool consumed);
void Clear();
inline constexpr std::string_view MenuName = "ModexGUIMenu";
}
