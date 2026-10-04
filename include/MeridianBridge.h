#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
namespace TextBridge::Meridian {
void Install();
void PollAutoFocus(bool enabled);
std::string AutoTarget();
bool HasFocus();
bool Begin(std::string_view autoTarget = {});
bool Session();
bool Valid();
void Cancel();
void Commit(const std::vector<std::uint32_t>& text);
}
