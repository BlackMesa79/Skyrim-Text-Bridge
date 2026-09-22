#pragma once
#include <cstdint>
#include <vector>
namespace TextBridge::Meridian {
void Install();
bool HasFocus();
bool Begin();
bool Session();
bool Valid();
void Cancel();
void Commit(const std::vector<std::uint32_t>& text);
}
