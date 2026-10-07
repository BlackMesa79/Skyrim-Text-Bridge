#pragma once
namespace TextBridge {
// Count only changes made inside Modex's synchronous render/message callbacks.
// Repair a leaked lease, never reset the shared engine count or other controls.
class MenuTextLease {
    int owned = 0;
    bool restore = false;
    int observed = -1;
public:
    void Observe(int before, int after) {
        if (observed >= 0 && before != observed) owned = 0;
        const auto delta = after - before;
        if (delta == 1 && owned == 0) owned = 1;
        else if (delta == -1 && owned == 1) owned = 0;
        else if (delta != 0) owned = 0; // ambiguous changes: decline repair
        observed = after;
    }
    bool Close(int current) {
        const bool repair = owned == 1 && current > 0;
        owned = 0; restore = repair;
        observed = repair ? current - 1 : current;
        return repair;
    }
    bool Reopen(int current) {
        const bool value = restore && current >= 0 && current < 127;
        restore = false; owned = value ? 1 : 0;
        observed = value ? current + 1 : current;
        return value;
    }
};
}
