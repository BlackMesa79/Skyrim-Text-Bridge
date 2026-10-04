#pragma once
#include <string>
#include <string_view>
namespace TextBridge {
class AutoInputPolicy {
public:
    enum class Action { None, Start, Stop };
private:
    std::string previous, blocked;
    bool owned = false;
public:
    bool OwnsMode() const { return owned; }
    void Interrupted() { owned = false; }
    void Reset() { previous.clear(); blocked.clear(); owned = false; }
    Action Update(std::string_view target, bool permitted, bool modeOn, bool manualToggle) {
        if (!permitted) { const bool stop = owned && modeOn; Reset(); return stop ? Action::Stop : Action::None; }
        const bool changed = target != previous;
        if (changed) blocked.clear();
        if (owned && !modeOn && !changed) { blocked = previous; owned = false; }
        if (owned && changed) { owned = false; previous = target; return modeOn ? Action::Stop : Action::None; }
        previous = target;
        if (!manualToggle && !modeOn && !target.empty() && target != blocked) return Action::Start;
        return Action::None;
    }
    void Started(std::string_view target) { previous = target; owned = !target.empty(); }
    void Failed(std::string_view target) { previous = target; blocked = target; owned = false; }
    void Manual(std::string_view target, bool on) {
        previous = target; owned = on && !target.empty();
        blocked = on ? std::string{} : std::string(target);
    }
};
}
