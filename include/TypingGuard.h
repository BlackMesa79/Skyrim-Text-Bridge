#pragma once
#include <bitset>
namespace TextBridge {
// Remember consumed presses until release, including releases after F8 exits.
class TypingGuard {
    std::bitset<256> swallowed;
    std::bitset<256> forwarded;
public:
    bool Pending() const { return swallowed.any(); }
    void Clear() { swallowed.reset(); forwarded.reset(); }
    bool Filter(unsigned scan, bool up, bool consume) {
        if (scan >= swallowed.size()) return consume;
        // A press already delivered before entering typing mode needs its release
        // to avoid leaving movement or a modifier stuck in the recipient.
        const bool drop = (consume || swallowed[scan]) && !(up && forwarded[scan]);
        if (up) { swallowed.reset(scan); forwarded.reset(scan); }
        else if (drop) swallowed.set(scan);
        else forwarded.set(scan);
        return drop;
    }
};
inline bool EditingKey(unsigned scan) {
    switch (scan) {
    case 0x0e: case 0x1c: case 0x9c: case 0x01: case 0x0f:
    case 0xc7: case 0xc8: case 0xc9: case 0xcb: case 0xcd:
    case 0xcf: case 0xd0: case 0xd1: case 0xd3: return true;
    default: return false;
    }
}
}
