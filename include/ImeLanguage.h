#pragma once
#include <Windows.h>
#include <imm.h>
#include <msctf.h>
#include <optional>
namespace TextBridge::ImeLanguage {
// Korean IMEs use the conversion bit for Hangul/Latin; IMM open status is not
// a reliable mode gate. Japanese can expose open/closed without conversion data.
inline std::optional<bool> NativeState(LANGID language, std::optional<bool> open, std::optional<DWORD> conversion) {
    if (PRIMARYLANGID(language) == LANG_KOREAN && conversion) return (*conversion & IME_CMODE_NATIVE) != 0;
    if (open && !*open) return false;
    if (conversion) return (*conversion & IME_CMODE_NATIVE) != 0;
    if (PRIMARYLANGID(language) == LANG_JAPANESE && open) return *open;
    return {};
}
// Read only the existing window-thread manager; absence is not an English
// signal. Modern TSF IMEs may expose state without matching IMM conversion.
inline std::optional<bool> TsfNativeMode(LANGID language = LOWORD(reinterpret_cast<ULONG_PTR>(GetKeyboardLayout(0)))) {
    using GetManager = HRESULT(WINAPI*)(ITfThreadMgr**);
    auto module = GetModuleHandleW(L"msctf.dll");
    auto get = module ? reinterpret_cast<GetManager>(GetProcAddress(module, "TF_GetThreadMgr")) : nullptr;
    ITfThreadMgr* manager{};
    if (!get || FAILED(get(&manager)) || !manager) return {};
    ITfCompartmentMgr* compartments{};
    std::optional<bool> result;
    if (SUCCEEDED(manager->QueryInterface(IID_PPV_ARGS(&compartments)))) {
        auto read = [&](const GUID& id) -> std::optional<LONG> {
            ITfCompartment* value{};
            std::optional<LONG> number;
            if (SUCCEEDED(compartments->GetCompartment(id, &value))) {
                VARIANT mode{};
                if (SUCCEEDED(value->GetValue(&mode)) && mode.vt == VT_I4) number = mode.lVal;
                VariantClear(&mode); value->Release();
            }
            return number;
        };
        const auto open = read(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE);
        const auto conversion = read(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION);
        result = NativeState(language, open ? std::optional<bool>(*open != 0) : std::nullopt,
            conversion ? std::optional<DWORD>(static_cast<DWORD>(*conversion)) : std::nullopt);
        compartments->Release();
    }
    manager->Release(); return result;
}
inline DWORD Conversion(DWORD current, bool english) {
    return english ? current & ~(IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_SYMBOL) : current | IME_CMODE_NATIVE;
}
// Use the existing window-thread TSF manager. Do not Activate/Deactivate it,
// select another keyboard profile, or acquire another UI's document focus.
inline void SetTsfMode(bool english, std::optional<DWORD> nativeConversion = {}) {
    using GetManager = HRESULT(WINAPI*)(ITfThreadMgr**);
    auto module = GetModuleHandleW(L"msctf.dll");
    auto get = module ? reinterpret_cast<GetManager>(GetProcAddress(module, "TF_GetThreadMgr")) : nullptr;
    ITfThreadMgr* manager{};
    if (!get || FAILED(get(&manager)) || !manager) return;
    ITfClientId* clients{}; ITfCompartmentMgr* compartments{};
    // Stable application identity, used solely for writing thread compartments.
    constexpr GUID identity{0x701f23a1,0x72d5,0x4680,{0x97,0x72,0x32,0x7f,0x5c,0xa7,0x41,0x91}};
    TfClientId client{};
    if (SUCCEEDED(manager->QueryInterface(IID_PPV_ARGS(&clients))) &&
        SUCCEEDED(clients->GetClientId(identity, &client)) &&
        SUCCEEDED(manager->QueryInterface(IID_PPV_ARGS(&compartments)))) {
        ITfCompartment* value{};
        if (SUCCEEDED(compartments->GetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &value))) {
            VARIANT mode{};
            if (SUCCEEDED(value->GetValue(&mode)) && mode.vt == VT_I4) {
                const DWORD source = !english && nativeConversion ? *nativeConversion : static_cast<DWORD>(mode.lVal);
                mode.lVal = static_cast<LONG>(Conversion(source, english));
                value->SetValue(client, &mode);
            }
            VariantClear(&mode); value->Release();
        }
        value = nullptr;
        if (SUCCEEDED(compartments->GetCompartment(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE, &value))) {
            VARIANT open{}; open.vt = VT_I4; open.lVal = english ? 0 : 1;
            value->SetValue(client, &open); value->Release();
        }
    }
    if (compartments) compartments->Release();
    if (clients) clients->Release();
    manager->Release();
}
inline void SetImmMode(HIMC context, bool english) {
    if (!context) return;
    DWORD conversion{}, sentence{};
    if (ImmGetConversionStatus(context, &conversion, &sentence))
        ImmSetConversionStatus(context, Conversion(conversion, english), sentence);
    ImmSetOpenStatus(context, english ? FALSE : TRUE);
}
}
