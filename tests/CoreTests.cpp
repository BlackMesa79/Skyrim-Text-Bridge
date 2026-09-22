#include "TypingGuard.h"
#include "BridgeCore.h"
#include "RuntimeSupport.h"
#include <iostream>
#include <stdexcept>

int checks = 0;
void Check(bool condition, const char* name) {
    if (!condition) throw std::runtime_error(name);
    ++checks;
}
int main() {
    using namespace TextBridge;
    Check(SupportedRuntime(1,5,97,0), "SE 1.5.97 allowed");
    Check(SupportedRuntime(1,6,1170,0), "AE 1.6.1170 retained");
    Check(!SupportedRuntime(1,4,15,0) && !SupportedRuntime(1,6,1179,0) && !SupportedRuntime(1,7,104,0), "unvalidated VR/GOG/new AE rejected");
    Check(DecodeUtf16(u"\u4e2d\u6587Ab").size() == 4, "Chinese and ASCII");
    Check(DecodeUtf16(u"\U0001f600")[0] == 0x1F600, "surrogate pair");
    Check(DecodeUtf16(std::u16string{0xD800, u'A', 0xDC00}) == std::vector<std::uint32_t>{65}, "invalid surrogates");
    Check(DecodeUtf16(u"\n\r\tA\x7f") == std::vector<std::uint32_t>{65}, "no control-key injection");
    Check(JsString(u"'\"\\\n") == "\"\\u0027\\u0022\\u005c\\u000a\"", "JS injection escaping");
    Check(JsString(u"\u2028\u2029") == "\"\\u2028\\u2029\"", "JS line separators");
    CharacterQueue queue;
    Check(queue.Begin(u"123456789\u4e2d"), "begin transaction");
    Check(!queue.Begin(u"other"), "reject overlapping transaction");
    Check(queue.Take(0).empty(), "full engine pool");
    Check(queue.Take(5).size() == 5, "first engine batch");
    auto last = queue.Take(5);
    Check(last.size() == 5 && last.back() == 0x4E2D && queue.Empty(), "no text truncation");
    Check(!queue.Begin(std::u16string(MaxTextUnits + 1, u'x')), "bounded text");
    queue.Begin(u"cancel"); queue.Cancel();
    Check(queue.Empty(), "cancel clears pending text");
    SessionGate gate;
    auto id = gate.Start(100);
    Check(id && !gate.Start(101), "one capture at a time");
    Check(gate.Move(id, SessionPhase::Capturing, SessionPhase::Editing), "open composer");
    Check(!gate.ExpireCapture(60000, 5000), "long composition never trips capture timeout");
    Check(gate.Move(id, SessionPhase::Editing, SessionPhase::Returning), "submit starts return phase");
    Check(!gate.Move(id, SessionPhase::Editing, SessionPhase::Returning), "duplicate submission rejected");
    Check(!gate.ExpireCapture(120000, 5000), "queued result survives old timeout regression");
    Check(gate.Finish(id), "return completes");
    auto second = gate.Start(130000);
    Check(second != id && !gate.Finish(id), "late old callback cannot close new session");
    Check(!gate.Move(id, SessionPhase::Capturing, SessionPhase::Editing), "late capture ignored");
    Check(gate.ExpireCapture(135001, 5000) == second && !gate.Active(), "real capture timeout unblocks input");
    auto third = gate.Start(140000); gate.Cancel();
    Check(!gate.Is(third, SessionPhase::Capturing), "load game cancels target capture");
    Check(!CompositionStillActive(true, 0), "empty composition overrides stale start event");
    Check(CompositionStillActive(false, 2), "pending character prevents premature submission");
    Check(CompositionStillActive(true, -1) && !CompositionStillActive(false, -1), "IMM error preserves event state");
    TextBridge::TypingGuard guard;
    Check(guard.Filter(0x1e,false,true), "typing consumes hotkey press");
    Check(guard.Filter(0x1e,false,false), "held key stays suppressed after exit");
    Check(guard.Filter(0x1e,true,false) && !guard.Pending(), "release after exit is consumed");
    Check(!guard.Filter(0x1e,false,false), "next press works normally");
    Check(guard.Filter(0x1e,false,true) && !guard.Filter(0x1e,true,true), "preexisting press receives balancing release to avoid stuck movement");
    Check(guard.Filter(0x3a,true,true), "release hotkeys blocked while typing");
    Check(TextBridge::EditingKey(0xd3) && TextBridge::EditingKey(0x0e) && !TextBridge::EditingKey(0x3a), "editing exceptions preserve deletion");
    std::cout << checks << " core checks passed\n";
}
