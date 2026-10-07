#pragma once
#include <atomic>
#include <cstdint>
namespace TextBridge {
// UI messages can arrive after the dispatch that ended typing. Keep that
// physical Esc local through release; a fresh ordinary press immediately re-arms.
class EscapeFence {
    std::atomic<std::uint64_t> until{};
public:
    void Observe(bool up, bool consumed, std::uint64_t now) {
        if (!up) until = consumed ? now + 2000 : 0;
        else if (until.load()) until = now + 100;
    }
    bool Blocks(std::uint64_t now) const { return now < until.load(); }
    void Clear() { until = 0; }
};
}
