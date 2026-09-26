#pragma once
#include <Windows.h>
#include <imm.h>
#include <msctf.h>
#include <string>
namespace TextBridge::InputMethodName {
inline std::wstring Clean(std::wstring value) {
    for (auto& c : value) if (c < L' ' || c == 0x7f) c = L' ';
    const auto first = value.find_first_not_of(L' ');
    if (first == std::wstring::npos) return {};
    value = value.substr(first, value.find_last_not_of(L' ') - first + 1);
    if (value.size() > 160) value.resize(160);
    return value;
}
// Call on the game window's thread. Query only; never activate a TSF profile.
inline std::wstring Read() {
    std::wstring name;
    const auto init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(init) || init == RPC_E_CHANGED_MODE) {
        ITfInputProcessorProfileMgr* manager{};
        if (SUCCEEDED(CoCreateInstance(CLSID_TF_InputProcessorProfiles, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&manager)))) {
            TF_INPUTPROCESSORPROFILE active{};
            if (manager->GetActiveProfile(GUID_TFCAT_TIP_KEYBOARD, &active) == S_OK &&
                active.dwProfileType == TF_PROFILETYPE_INPUTPROCESSOR) {
                ITfInputProcessorProfiles* profiles{};
                if (SUCCEEDED(manager->QueryInterface(IID_PPV_ARGS(&profiles)))) {
                    BSTR description{};
                    if (profiles->GetLanguageProfileDescription(active.clsid, active.langid, active.guidProfile, &description) == S_OK && description)
                        name.assign(description, SysStringLen(description));
                    SysFreeString(description);
                    profiles->Release();
                }
            }
            manager->Release();
        }
        if (SUCCEEDED(init)) CoUninitialize();
    }
    name = Clean(name);
    if (!name.empty()) return name;
    const auto layout = GetKeyboardLayout(0);
    wchar_t description[256]{};
    if (ImmGetDescriptionW(layout, description, 256)) name = Clean(description);
    if (!name.empty()) return name;
    // A language label is explicitly a fallback, not a guessed IME brand.
    if (GetLocaleInfoW(MAKELCID(LOWORD(reinterpret_cast<ULONG_PTR>(layout)), SORT_DEFAULT),
        LOCALE_SLOCALIZEDDISPLAYNAME, description, 256))
        return L"键盘 / 输入法名称不可用（" + Clean(description) + L"）";
    return L"输入法名称不可用";
}
}
