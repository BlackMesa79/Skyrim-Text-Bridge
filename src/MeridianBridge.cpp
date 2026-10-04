#include <SKSE/SKSE.h>
#include <Windows.h>
#include "MeridianBridge.h"
#include "MeridianApi.h"
#include "BridgeCore.h"
#include "MeridianBridge.generated.h"
#include "../extern/MeridianUI/ViewAPI.h"
#include <atomic>
#include <mutex>
#include <charconv>
namespace TextBridge::Meridian {
namespace {
using API=::Meridian::UI::View::IViewAPI;
using View=::Meridian::UI::View::ViewHandle;
using Result=::Meridian::UI::View::FocusResult;
using Mode=::Meridian::UI::View::FocusMode;
using FocusFn=Result (*)(API*,View,Mode);
API* api{}; FocusFn originalFocus{};
std::atomic<View> focused{0},captured{0}; std::atomic<std::uint64_t> sequence{0};
std::atomic<bool> rejected{false}; std::mutex dispatchMutex;
View monitored{};
std::mutex focusMutex;
std::uint64_t monitorEpoch{}, fieldId{};
ULONGLONG focusReportedAt{}, lastProbe{}, nextMonitorAttempt{};
void AutoStatus(const char* payload) {
    if (!payload) return;
    std::string_view value(payload);
    auto separator = value.find(':'); if (separator == std::string_view::npos) return;
    std::uint64_t epoch{}, field{};
    auto a = std::from_chars(value.data(), value.data()+separator, epoch);
    auto b = std::from_chars(value.data()+separator+1, value.data()+value.size(), field);
    if (a.ec != std::errc{} || a.ptr != value.data()+separator || b.ec != std::errc{} ||
        b.ptr != value.data()+value.size()) return;
    std::lock_guard lock(focusMutex);
    if (epoch != monitorEpoch) return;
    fieldId = field; focusReportedAt = GetTickCount64();
}
void StopAutoObserver() {
    const auto old = std::exchange(monitored, 0);
    std::uint64_t oldEpoch{};
    {
        std::lock_guard lock(focusMutex);
        oldEpoch = monitorEpoch++; fieldId = 0; focusReportedAt = 0;
    }
    lastProbe = 0;
    if (api && old && api->IsReady(old)) {
        auto script = "if(window.__stbMeridian)window.__stbMeridian.stopObserve("+std::to_string(oldEpoch)+");";
        api->ExecuteJavaScript(old, script.c_str());
    }
}
Result Focus(API* self,View view,Mode mode) {
    auto result=originalFocus(self,view,mode);
    if(result==Result::Granted || result==Result::AlreadyFocused) focused=view;
    return result;
}
void Status(const char* payload) {
    if(!payload)return;
    const auto prefix=std::to_string(sequence.load())+":";
    std::string_view result(payload);
    // Meridian listener payloads are strings (not code); never log entered text.
    if(!result.starts_with(prefix)) return;
    result.remove_prefix(prefix.size());
    if(result!="ready" && result!="inserted") rejected=true;
    SKSE::log::info("Meridian IME: {}",result);
}
}
void Install() {
    if(api)return;
    auto module=GetModuleHandleW(L"MeridianUI.dll"); if(!module)return;
    auto query=reinterpret_cast<::Meridian::UI::View::QueryMeridianExtensionFn>(GetProcAddress(module,"QueryMeridianExtension"));
    api=RequestMeridianView(query);
    if(!api) {SKSE::log::warn("Meridian View/1 unavailable");return;}
    // MSVC x64 View/1: deleting destructor slot 0, TryFocus slot 9.
    auto table=*reinterpret_cast<std::uintptr_t**>(api);
    originalFocus=reinterpret_cast<FocusFn>(table[9]);
    REL::safe_write(reinterpret_cast<std::uintptr_t>(&table[9]),reinterpret_cast<std::uintptr_t>(&Focus));
    SKSE::log::info("Meridian View/1 focus observer installed");
}
void PollAutoFocus(bool enable) {
    if (!enable) {
        if (monitored) StopAutoObserver();
        nextMonitorAttempt = 0; return;
    }
    const auto view = focused.load();
    if (!api || !view || !api->IsReady(view) || !api->HasFocus(view)) {
        if (monitored) StopAutoObserver();
        nextMonitorAttempt = 0;
        return;
    }
    const auto now = GetTickCount64();
    if (monitored != view) {
        if (monitored) StopAutoObserver();
        if (now < nextMonitorAttempt) return;
        nextMonitorAttempt = now + 500;
        monitored = view;
        std::uint64_t epoch{};
        {
            std::lock_guard lock(focusMutex);
            epoch = ++monitorEpoch; fieldId = 0; focusReportedAt = 0;
        }
        if (!api->RegisterListener(view, "stbMeridianFocus", AutoStatus)) { StopAutoObserver(); return; }
        std::string script = MeridianScript;
        script += ";window.__stbMeridian.observe("+std::to_string(epoch)+");";
        if (!api->ExecuteJavaScript(view, script.c_str())) { StopAutoObserver(); return; }
    }
    if (!lastProbe || now-lastProbe >= 50) {
        lastProbe = now;
        std::uint64_t epoch{};
        { std::lock_guard lock(focusMutex); epoch = monitorEpoch; }
        auto script="if(window.__stbMeridian)window.__stbMeridian.probe("+std::to_string(epoch)+");";
        if (!api->ExecuteJavaScript(view,script.c_str())) {
            std::lock_guard lock(focusMutex); fieldId = 0; focusReportedAt = 0;
        }
    }
}
std::string AutoTarget() {
    std::uint64_t field{}, epoch{}; ULONGLONG reported{};
    {
        std::lock_guard lock(focusMutex);
        field = fieldId; epoch = monitorEpoch; reported = focusReportedAt;
    }
    if (!monitored || !field || !reported || GetTickCount64()-reported > 500 || !api ||
        focused.load()!=monitored || !api->IsReady(monitored) || !api->HasFocus(monitored)) return {};
    return "mer:"+std::to_string(monitored)+":"+std::to_string(epoch)+":"+std::to_string(field);
}
bool HasFocus() {return api && api->HasAnyFocus();}
bool Session() {return captured.load()!=0;}
bool Valid() {auto view=captured.load();return api && view && !rejected && focused.load()==view && api->IsReady(view) && api->HasFocus(view);}
bool Begin(std::string_view autoTarget) {
    std::lock_guard lock(dispatchMutex);
    std::uint64_t expectedField{};
    if (!autoTarget.empty()) {
        if (autoTarget != AutoTarget()) return false;
        auto last = autoTarget.rfind(':');
        if (last == std::string_view::npos) return false;
        auto number = std::from_chars(autoTarget.data()+last+1, autoTarget.data()+autoTarget.size(), expectedField);
        if (number.ec != std::errc{} || number.ptr != autoTarget.data()+autoTarget.size() || !expectedField) return false;
    }
    auto view=focused.load();
    if(!api || !view || !api->IsReady(view) || !api->HasFocus(view)) {SKSE::log::warn("Meridian active view not observed; close and reopen its menu");return false;}
    auto id=++sequence; captured=view; rejected=false;
    if(!api->RegisterListener(view,"stbMeridianResult",Status)) {captured=0;return false;}
    std::string script=MeridianScript;
    script+=";window.__stbMeridian.capture("+std::to_string(id)+","+std::to_string(expectedField)+");";
    if(!api->ExecuteJavaScript(view,script.c_str())) {captured=0;return false;}
    SKSE::log::info("Meridian IME target captured asynchronously, session {}",id);return true;
}
void Cancel() {
    std::lock_guard lock(dispatchMutex);
    auto view=captured.exchange(0); auto id=sequence.load();
    if(api && view && api->IsReady(view)) {
        auto script="if(window.__stbMeridian)window.__stbMeridian.cancel("+std::to_string(id)+");";
        api->ExecuteJavaScript(view,script.c_str());
    }
}
void Commit(const std::vector<std::uint32_t>& text) {
    if(text.empty())return;
    std::lock_guard lock(dispatchMutex);
    if(!Valid())return;
    std::u16string units;
    for(auto c:text) {if(c<=0xffff)units.push_back(static_cast<char16_t>(c));else if(c<=0x10ffff) {c-=0x10000;units.push_back(0xd800+(c>>10));units.push_back(0xdc00+(c&1023));}}
    auto script="if(window.__stbMeridian)window.__stbMeridian.commit("+std::to_string(sequence.load())+","+JsString(units)+");";
    if(!api->ExecuteJavaScript(captured.load(),script.c_str())) rejected=true;
}
}
