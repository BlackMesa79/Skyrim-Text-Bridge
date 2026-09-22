#include <SKSE/SKSE.h>
#include <Windows.h>
#include "MeridianBridge.h"
#include "MeridianApi.h"
#include "BridgeCore.h"
#include "MeridianBridge.generated.h"
#include "../extern/MeridianUI/ViewAPI.h"
#include <atomic>
#include <mutex>
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
bool HasFocus() {return api && api->HasAnyFocus();}
bool Session() {return captured.load()!=0;}
bool Valid() {auto view=captured.load();return api && view && !rejected && focused.load()==view && api->IsReady(view) && api->HasFocus(view);}
bool Begin() {
    std::lock_guard lock(dispatchMutex);
    auto view=focused.load();
    if(!api || !view || !api->IsReady(view) || !api->HasFocus(view)) {SKSE::log::warn("Meridian active view not observed; close and reopen its menu");return false;}
    auto id=++sequence; captured=view; rejected=false;
    if(!api->RegisterListener(view,"stbMeridianResult",Status)) {captured=0;return false;}
    std::string script=MeridianScript;
    script+=";window.__stbMeridian.capture("+std::to_string(id)+");";
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
