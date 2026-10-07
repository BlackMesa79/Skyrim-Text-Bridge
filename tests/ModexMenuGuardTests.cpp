// Execute the production menu-message guard with minimal engine data doubles.
// No Skyrim DLL, vtable memory patch, or actual Modex DLL is executed here.
#define TEXTBRIDGE_MODEX_TEST
#include <Windows.h>
#include <string>
#include <iostream>
#include <cstdlib>
namespace RE {
enum class UI_MESSAGE_RESULTS { kHandled, kIgnore, kPassOn };
enum class UI_MESSAGE_TYPE { kUpdate, kShow, kHide, kForceHide, kScaleformEvent, kUserEvent };
struct IMenu {};
struct GFxEvent { enum class EventType { kKeyDown, kKeyUp, kCharEvent }; EventType type; };
namespace GFxKey { enum Code { kEscape, kReturn }; }
struct GFxKeyEvent : GFxEvent { GFxKey::Code keyCode; };
struct BSUIScaleformData { GFxEvent* scaleformEvent; };
struct BSUIMessageData { std::string fixedStr; };
struct UIMessage { UI_MESSAGE_TYPE type; void* data; };
}
namespace { bool active{}, prisma{}; unsigned forwarded{}, closed{}, checks{}; }
namespace TextBridge::DirectIme { bool SuppressKeyboard() { return active; } }
namespace TextBridge::Prisma { bool OwnsInput() { return prisma; } }
namespace TextBridge::ModexMenuGuard { namespace {
int textCount = 0, renderDelta = 0, closeDelta = 0;
int TextCount() { return textCount; }
void TextPermission(bool allow) { textCount += allow ? 1 : -1; }
} }
#include "../src/ModexMenuGuard.cpp"
RE::UI_MESSAGE_RESULTS Peer(RE::IMenu*, RE::UIMessage& message) {
    ++forwarded;
    if (message.type == RE::UI_MESSAGE_TYPE::kHide || message.type == RE::UI_MESSAGE_TYPE::kForceHide) ++closed;
    if (message.type == RE::UI_MESSAGE_TYPE::kHide) TextBridge::ModexMenuGuard::textCount += TextBridge::ModexMenuGuard::closeDelta;
    return RE::UI_MESSAGE_RESULTS::kPassOn;
}
void Check(bool ok, const char* name) { ++checks; if (!ok) { std::cerr << name << '\n'; std::exit(1); } }
void Render(RE::IMenu*) { TextBridge::ModexMenuGuard::textCount += TextBridge::ModexMenuGuard::renderDelta; }
int main() {
    using namespace TextBridge::ModexMenuGuard;
    using Type = RE::UI_MESSAGE_TYPE;
    using Result = RE::UI_MESSAGE_RESULTS;
    previous = Peer;
    previousDisplay = Render;
    RE::IMenu menu;
    RE::BSUIMessageData cancel{"Cancel"}, other{"Accept"};
    RE::UIMessage message{Type::kUserEvent, &cancel};
    Check(ProcessMessage(&menu, message) == Result::kPassOn && forwarded == 1, "idle Cancel forwards unchanged");
    active = true;
    Check(ProcessMessage(&menu, message) == Result::kHandled && forwarded == 1, "active Cancel never reaches Modex PopWindow");
    RE::GFxKeyEvent key{{RE::GFxEvent::EventType::kKeyDown}, RE::GFxKey::kEscape};
    RE::BSUIScaleformData data{&key};
    message = {Type::kScaleformEvent, &data};
    Check(ProcessMessage(&menu, message) == Result::kHandled, "separate Scaleform Esc down also stays local");
    key.type = RE::GFxEvent::EventType::kKeyUp;
    Check(ProcessMessage(&menu, message) == Result::kHandled, "Scaleform Esc release is balanced");
    key.keyCode = RE::GFxKey::kReturn;
    Check(ProcessMessage(&menu, message) == Result::kPassOn, "ordinary editing key reaches Modex");
    message = {Type::kUserEvent, &other};
    Check(ProcessMessage(&menu, message) == Result::kPassOn, "non-Cancel user event untouched");
    ObserveEscape(false, true); active = false;
    message = {Type::kUserEvent, &cancel};
    Check(ProcessMessage(&menu, message) == Result::kHandled, "queued Cancel stays local after completed Esc disables IME");
    ObserveEscape(true, true);
    Check(ProcessMessage(&menu, message) == Result::kHandled, "release tail survives asynchronous menu delivery");
    ObserveEscape(false, false);
    Check(ProcessMessage(&menu, message) == Result::kPassOn, "fresh ordinary Esc immediately closes menu even within tail grace");
    active = true;
    message = {Type::kHide, nullptr};
    Check(ProcessMessage(&menu, message) == Result::kPassOn && closed == 1, "normal hide always reaches Modex cleanup");
    message.type = Type::kForceHide;
    Check(ProcessMessage(&menu, message) == Result::kPassOn && closed == 2, "forced hide also reaches original handler");
    message = {Type::kUserEvent, nullptr};
    Check(ProcessMessage(&menu, message) == Result::kPassOn, "null message data forwards safely");
    message = {Type::kScaleformEvent, nullptr};
    Check(ProcessMessage(&menu, message) == Result::kPassOn, "null Scaleform data forwards safely");
    data.scaleformEvent = nullptr; message.data = &data;
    Check(ProcessMessage(&menu, message) == Result::kPassOn, "null Scaleform event forwards safely");
    ObserveEscape(false, true); prisma = true;
    message = {Type::kUserEvent, &cancel};
    Check(ProcessMessage(&menu, message) == Result::kPassOn, "Prisma bypasses both active and tail guard");
    prisma = false; active = false; Clear();
    Check(ProcessMessage(&menu, message) == Result::kPassOn, "focus/load reset leaves no Cancel interception");
    textCount = 2; renderDelta = 1; PostDisplay(&menu);
    Check(textCount == 3, "Modex render acquires one tracked text lease above other owners");
    message = {Type::kHide, nullptr}; ProcessMessage(&menu, message);
    Check(textCount == 2, "close repairs only Modex's leaked lease and preserves other owners");
    message.type = Type::kShow; ProcessMessage(&menu, message);
    Check(textCount == 3, "reopen restores lease for Modex's retained text flag");
    renderDelta = -1; PostDisplay(&menu);
    Check(textCount == 2, "next unfocused render releases restored lease without underflow");
    message.type = Type::kHide; ProcessMessage(&menu, message);
    Check(textCount == 2, "unfocused close never decrements another owner's count");
    message.type = Type::kShow; ProcessMessage(&menu, message);
    Check(textCount == 2, "reopen after clean close adds no synthetic lease");
    renderDelta = 1; PostDisplay(&menu); closeDelta = -1;
    message.type = Type::kHide; ProcessMessage(&menu, message);
    Check(textCount == 2, "Modex version that releases on hide is never double-decremented");
    closeDelta = 0; renderDelta = 2; PostDisplay(&menu);
    message.type = Type::kHide; ProcessMessage(&menu, message);
    Check(textCount == 4, "ambiguous render count changes decline repair");
    textCount = 0; renderDelta = 1; PostDisplay(&menu); textCount = 0;
    ProcessMessage(&menu, message);
    Check(textCount == 0, "external cleanup cannot cause text-count underflow");
    message.type = Type::kShow; ProcessMessage(&menu, message);
    renderDelta = 1; PostDisplay(&menu); textCount += 1;
    message.type = Type::kHide; ProcessMessage(&menu, message);
    Check(textCount == 2, "external ownership changes invalidate repair instead of decrementing an unproven owner");
    std::cout << checks << " production Modex menu guard checks passed (engine data doubles)\n";
}
