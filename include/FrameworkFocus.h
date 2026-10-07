#pragma once
#include <string>
namespace TextBridge::FrameworkFocus {
enum class Backend { None, Native, Scaleform, Meridian };
struct Target { Backend backend = Backend::None; std::string identity; };
void Poll(bool enabled);
Target Current();
bool Begin(const Target& target);
}
