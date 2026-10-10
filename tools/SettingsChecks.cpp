#include "../src/Settings.cpp"
#include "PanelState.h"
#include <iostream>
#include <limits>
#include <filesystem>
#include <fstream>
#include <iterator>
int main() {
    using namespace TextBridge;
    const auto root=std::filesystem::absolute("build/preview/settings-test-"+std::to_string(GetCurrentProcessId()));
    auto path=(root/"personal"/"SkyrimTextBridge.user.ini").wstring();
    const auto legacy=(root/"SkyrimTextBridge.ini").wstring();
    auto bytes=[](const std::wstring& file) { std::ifstream input(file,std::ios::binary); return std::string(std::istreambuf_iterator<char>(input),{}); };
    Settings::Load(path,legacy);
    const auto defaults=Settings::Get();
    if (!defaults.enabled || defaults.autoEnable || defaults.hotkey!=66 || defaults.language!=0 ||
        defaults.x!=.025f || defaults.y!=.065f || defaults.size!=1 || defaults.opacity!=.92f ||
        std::filesystem::exists(path) || std::filesystem::exists(legacy)) return 11;
    // Simulate a fully customized legacy file, then migrate without modifying it.
    Settings::Load(legacy);
    auto previous=Settings::Data{}; previous.enabled=false;previous.autoEnable=true;previous.hotkey=62;
    previous.language=3;previous.x=.7f;previous.y=.2f;previous.size=1.4f;previous.opacity=.6f;
    previous.background={.2f,.3f,.4f};previous.accent={.4f,.5f,.6f};previous.foreground={.6f,.7f,.8f};
    Settings::Set(previous); if (!Settings::Save()) return 12;
    const auto legacyBytes=bytes(legacy);
    Settings::Load(path,legacy);const auto migrated=Settings::Get();
    if (migrated.enabled!=previous.enabled || migrated.autoEnable!=previous.autoEnable || migrated.hotkey!=previous.hotkey ||
        migrated.language!=previous.language || migrated.x!=previous.x || migrated.y!=previous.y ||
        migrated.size!=previous.size || migrated.opacity!=previous.opacity || migrated.background!=previous.background ||
        migrated.accent!=previous.accent || migrated.foreground!=previous.foreground ||
        std::filesystem::exists(path) || bytes(legacy)!=legacyBytes) return 13;
    if (!Settings::Save() || !std::filesystem::exists(path) || bytes(legacy)!=legacyBytes) return 14;
    const auto personalBytes=bytes(path);
    Settings::Load(path,legacy); if (bytes(path)!=personalBytes) return 15;
    // Personal files are authoritative; missing keys use compiled defaults,
    // never legacy values. Unknown keys survive saving.
    WritePrivateProfileStringW(L"Future",L"Keep",L"unchanged",path.c_str());
    WritePrivateProfileStringW(L"Panel",L"Scale",nullptr,path.c_str());
    Settings::Load(path,legacy); if (Settings::Get().size!=1) return 16;
    if (!Settings::Save()) return 17;
    wchar_t preserved[32]{};GetPrivateProfileStringW(L"Future",L"Keep",L"",preserved,32,path.c_str());
    if (std::wstring(preserved)!=L"unchanged") return 18;
    Settings::Load(path); auto value=Settings::Data{};
    if (value.autoEnable) return 7;
    value.autoEnable=true; value.enabled=false; value.hotkey=65; value.language=4; value.x=.8f; value.y=.3f; value.opacity=.25f; value.background={.2f,.3f,.4f};
    Settings::Set(value); if (!Settings::Save()) return 1;
    Settings::Set({}); Settings::Load(path); auto loaded=Settings::Get();
    if (!loaded.autoEnable || loaded.enabled || loaded.hotkey!=65 || loaded.language!=4 || std::abs(loaded.opacity-.25f)>.0001f || loaded.background!=value.background) return 2;
    WritePrivateProfileStringW(L"Interface",L"Language",nullptr,path.c_str());
    Settings::Load(path); if (Settings::Get().language != 0) return 9;
    value.language=999; Settings::Set(value); if (Settings::Get().language != 0) return 10;
    WritePrivateProfileStringW(L"Input",L"AutoEnable",nullptr,path.c_str());
    Settings::Load(path); if (Settings::Get().autoEnable) return 8;
    value.size=std::numeric_limits<float>::quiet_NaN(); value.opacity=2; value.x=-3;
    Settings::Set(value); auto clamped=Settings::Get(); if (clamped.size!=1 || clamped.opacity!=1 || clamped.x!=0) return 3;
    for (auto opacity : {0.f,.5f,1.f}) {
        value=Settings::Data{}; value.opacity=opacity; Settings::Set(value);
        if ((PanelColor(.2f,.3f,.4f,Settings::Get().opacity) >> 24) != unsigned(opacity*255+.5f)) return 4;
        if ((PanelColor(.9f,.9f,.9f) >> 24) != 255) return 5;
    }
    const auto blocker=root/L"not-a-directory";
    { std::ofstream output(blocker);output<<"blocker"; }
    Settings::Load((blocker/L"settings.ini").wstring());
    if (Settings::Save()) return 19;
    std::cout << "No-INI initialization, complete legacy migration, personal-file priority, read-only load, first-save creation, unknown-key preservation, save failure, language persistence, clamps and alpha checks passed\n";
}
