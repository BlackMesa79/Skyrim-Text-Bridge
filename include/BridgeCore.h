#pragma once
#include <cstdint>
#include <deque>
#include <string>
#include <string_view>
#include <vector>
#include <mutex>

namespace TextBridge {
inline constexpr std::size_t MaxTextUnits = 1024;

enum class SessionPhase { Idle, Capturing, Editing, Returning };
// The capture deadline does not apply while composing, or while the result is
// waiting for a game task. Every asynchronous callback carries its session ID.
class SessionGate {
public:
    std::uint64_t Start(std::uint64_t now) {
        std::lock_guard lock(mutex);
        if (phase != SessionPhase::Idle) return 0;
        phase = SessionPhase::Capturing; started = now; return ++id;
    }
    bool Move(std::uint64_t request, SessionPhase from, SessionPhase to) {
        std::lock_guard lock(mutex);
        if (id != request || phase != from) return false;
        phase = to; return true;
    }
    bool Is(std::uint64_t request, SessionPhase expected) const {
        std::lock_guard lock(mutex); return id == request && phase == expected;
    }
    bool Active() const {
        std::lock_guard lock(mutex); return phase != SessionPhase::Idle;
    }
    bool Finish(std::uint64_t request) {
        std::lock_guard lock(mutex);
        if (id != request || phase == SessionPhase::Idle) return false;
        phase = SessionPhase::Idle; return true;
    }
    std::uint64_t ExpireCapture(std::uint64_t now, std::uint64_t limit) {
        std::lock_guard lock(mutex);
        if (phase != SessionPhase::Capturing || now - started <= limit) return 0;
        phase = SessionPhase::Idle; return id;
    }
    void Cancel() { std::lock_guard lock(mutex); phase = SessionPhase::Idle; }
private:
    mutable std::mutex mutex;
    SessionPhase phase = SessionPhase::Idle;
    std::uint64_t id = 0, started = 0;
};

inline bool CompositionStillActive(bool eventState, long byteCount) {
    // A successful zero-length query overrides a stale STARTCOMPOSITION flag.
    // Negative IMM errors are inconclusive: keep the conservative event state.
    return byteCount >= 0 ? byteCount > 0 : eventState;
}

// Skyrim's character event carries one Unicode scalar; never emit half a surrogate.
inline std::vector<std::uint32_t> DecodeUtf16(std::u16string_view input)
{
    std::vector<std::uint32_t> out;
    for (std::size_t i = 0; i < input.size(); ++i) {
        std::uint32_t c = input[i];
        if (c >= 0xD800 && c <= 0xDBFF) {
            if (i + 1 >= input.size() || input[i + 1] < 0xDC00 || input[i + 1] > 0xDFFF) continue;
            c = 0x10000 + ((c - 0xD800) << 10) + (input[++i] - 0xDC00);
        } else if (c >= 0xDC00 && c <= 0xDFFF) continue;
        // This prototype sends single-line text only, never control keys.
        if (c >= 0x20 && c != 0x7F) out.push_back(c);
    }
    return out;
}

// ASCII-only JS string encoding: source text can never become executable script.
inline std::string JsString(std::u16string_view input)
{
    constexpr char hex[] = "0123456789abcdef";
    std::string out = "\"";
    for (auto c : input) {
        out += "\\u";
        out += hex[(c >> 12) & 15]; out += hex[(c >> 8) & 15];
        out += hex[(c >> 4) & 15]; out += hex[c & 15];
    }
    return out + '"';
}

class CharacterQueue {
public:
    bool Begin(std::u16string_view input) {
        if (!pending.empty() || input.size() > MaxTextUnits) return false;
        auto text = DecodeUtf16(input);
        pending.assign(text.begin(), text.end());
        return !pending.empty();
    }
    std::vector<std::uint32_t> Take(std::size_t capacity) {
        std::vector<std::uint32_t> out;
        while (capacity-- && !pending.empty()) {
            out.push_back(pending.front()); pending.pop_front();
        }
        return out;
    }
    void Cancel() { pending.clear(); }
    bool Empty() const { return pending.empty(); }
private:
    std::deque<std::uint32_t> pending;
};
}
