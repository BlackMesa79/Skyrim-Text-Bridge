#include <SKSE/SKSE.h>
#include <RE/I/InputEvent.h>
#include "Settings.h"
#include "DirectIme.h"
#include "Localization.h"
#include "../extern/SKSEMenuFramework/SKSEMenuFramework.h"
#include <atomic>
#include <optional>
namespace TextBridge::SettingsMenu {
namespace {
std::atomic<bool> capturing{false}, menuOpen{false}; bool installed=false;
std::optional<Localization::Key> feedback;
using Localization::Key;
void __stdcall Render() {
    auto value=Settings::Get(); bool changed=false;
    const auto selected = Localization::Current();
    const auto font = ImGuiMCP::GetFont();
    const ImGuiMCP::ImWchar sample = selected == Localization::Language::Chinese ? 0x4E2D :
        selected == Localization::Language::Japanese ? 0x3042 : selected == Localization::Language::Korean ? 0xAC00 : 0;
    const bool missing = sample && font && !ImGuiMCP::ImFontManger::FindGlyphNoFallback(font, sample);
    // Keep the selector readable if the selected script is not loaded by the
    // framework. The preference is retained, not silently changed to English.
    const auto language = missing ? Localization::Language::English : selected;
    auto text = [&](Key key) { return Localization::Text(key, language); };
    auto id = [&](Key key, const char* stable) { return text(key) + "###" + stable; };
    auto hint = [&](Key key) { ImGuiMCP::TextWrapped("%s", text(key).c_str()); };
    ImGuiMCP::TextUnformatted("Skyrim Text Bridge");
    const auto automatic = text(Key::AutoLanguage);
    const char* languages[]{automatic.c_str(), "English", "Chinese (zh-CN)", "Japanese (ja)", "Korean (ko)"};
    changed |= ImGuiMCP::Combo(id(Key::LanguageLabel,"stb-language").c_str(),&value.language,languages,5);
    if (missing) hint(Key::FontMissing);
    changed |= ImGuiMCP::Checkbox(id(Key::Enabled,"stb-enabled").c_str(),&value.enabled);
    changed |= ImGuiMCP::Checkbox(id(Key::AutoEnable,"stb-auto").c_str(),&value.autoEnable);
    hint(Key::AutoHint);
    hint(Key::ControlsHint);
    ImGuiMCP::TextUnformatted((text(Key::Hotkey) + Localization::Utf8(Settings::KeyName(value.hotkey))).c_str());
    if (ImGuiMCP::Button(id(capturing ? Key::CaptureHotkey : Key::ChangeHotkey,"stb-hotkey").c_str())) capturing = !capturing.load();
    ImGuiMCP::Separator();
    ImGuiMCP::TextUnformatted(text(Key::Layout).c_str());
    changed |= ImGuiMCP::SliderFloat(id(Key::X,"stb-x").c_str(),&value.x,0,1,"%.2f");
    changed |= ImGuiMCP::SliderFloat(id(Key::Y,"stb-y").c_str(),&value.y,0,1,"%.2f");
    changed |= ImGuiMCP::SliderFloat(id(Key::Scale,"stb-scale").c_str(),&value.size,.6f,1.8f,"%.2fx");
    ImGuiMCP::Separator();
    changed |= ImGuiMCP::ColorEdit3(id(Key::Background,"stb-background").c_str(),value.background.data());
    changed |= ImGuiMCP::ColorEdit3(id(Key::Accent,"stb-accent").c_str(),value.accent.data());
    changed |= ImGuiMCP::ColorEdit3(id(Key::TextColor,"stb-text").c_str(),value.foreground.data());
    changed |= ImGuiMCP::SliderFloat(id(Key::Opacity,"stb-opacity").c_str(),&value.opacity,0,1,"%.2f");
    hint(Key::FontHint);
    if (changed) { Settings::Set(value); feedback=Key::Applied; }
    if (ImGuiMCP::Button(id(Key::Save,"stb-save").c_str())) feedback=Settings::Save()?Key::Saved:Key::SaveFailed;
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button(id(Key::Reset,"stb-reset").c_str())) { Settings::Set({}); feedback=Key::ResetDone; }
    if (feedback) ImGuiMCP::TextUnformatted(text(*feedback).c_str());
}
void __stdcall Event(SKSEMenuFramework::Model::EventType type) {
    if (type==SKSEMenuFramework::Model::kOpenMenu) { menuOpen=true; DirectIme::Reset(); }
    if (type==SKSEMenuFramework::Model::kCloseMenu) { menuOpen=false; capturing=false; }
}
}
void Install() {
    const auto module=GetModuleHandleW(L"SKSEMenuFramework.dll");
    if (!module) {
        SKSE::log::info("Menu Framework unavailable; settings remain accessible via user INI"); return;
    }
    for (auto name : {"AddSectionItem","IsAnyBlockingWindowOpened","RegisterEvent","igTextUnformatted","igCheckbox","igButton","igSeparator","igSliderFloat","igColorEdit3","igSameLine","igCombo_Str_arr","igTextWrappedV","igGetFont","ImFont_FindGlyphNoFallback"})
        if (!GetProcAddress(module,name)) { SKSE::log::warn("Menu Framework settings disabled: missing {}",name); return; }
    SKSEMenuFramework::SetSection("Skyrim Text Bridge");
    // The public API has no rename/remove operation. Translate on registration;
    // a language change updates page contents immediately, the sidebar on restart.
    const auto label = Localization::Text(Key::Settings);
    // '/' is a hierarchy separator in Menu Framework, not a visual delimiter.
    SKSEMenuFramework::AddSectionItem(label,Render);
    using RegisterEvent=std::int64_t (*)(SKSEMenuFramework::Model::EventCallback);
    static auto listener = reinterpret_cast<RegisterEvent>(GetProcAddress(module,"RegisterEvent"))(Event);
    installed=true; SKSE::log::info("Menu Framework settings registered");
}
bool Blocking() { return installed && (menuOpen || SKSEMenuFramework::IsAnyBlockingWindowOpened()); }
bool CaptureKey(unsigned scan) {
    if (!capturing || scan==0 || scan>255) return false;
    capturing=false;
    if (scan!=1) { auto value=Settings::Get(); value.hotkey=scan; Settings::Set(value); }
    return true;
}
}
