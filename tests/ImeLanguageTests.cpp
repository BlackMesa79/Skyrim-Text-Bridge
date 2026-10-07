#include "ImeLanguage.h"
#include <iostream>
#include <cstdlib>
unsigned checks{};
void Check(bool ok, const char* name) { ++checks; if (!ok) { std::cerr << name << '\n'; std::exit(1); } }
int main() {
    using namespace TextBridge::ImeLanguage;
    Check(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)), "initialize isolated test apartment");
    ITfThreadMgr* manager{};
    Check(SUCCEEDED(CoCreateInstance(CLSID_TF_ThreadMgr, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&manager))), "create isolated real TSF manager");
    TfClientId client{};
    Check(SUCCEEDED(manager->Activate(&client)), "activate only test process's manager");
    ITfCompartmentMgr* compartments{};
    Check(SUCCEEDED(manager->QueryInterface(IID_PPV_ARGS(&compartments))), "real thread compartment interface");
    ITfCompartment* conversion{}; ITfCompartment* open{};
    Check(SUCCEEDED(compartments->GetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &conversion)) &&
          SUCCEEDED(compartments->GetCompartment(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE, &open)), "real mode compartments available");
    VARIANT value{}; value.vt = VT_I4; value.lVal = IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN;
    Check(SUCCEEDED(conversion->SetValue(client, &value)), "seed native conversion in isolated manager");
    value.lVal = 1; Check(SUCCEEDED(open->SetValue(client, &value)), "seed open state");
    SetTsfMode(true);
    VARIANT actual{};
    Check(SUCCEEDED(conversion->GetValue(&actual)) && actual.vt == VT_I4 && actual.lVal == IME_CMODE_ROMAN, "production TSF setter really clears native and fullwidth");
    VariantClear(&actual);
    Check(SUCCEEDED(open->GetValue(&actual)) && actual.vt == VT_I4 && actual.lVal == 0, "production TSF setter closes IME");
    VariantClear(&actual);
    SetTsfMode(false);
    Check(SUCCEEDED(conversion->GetValue(&actual)) && actual.vt == VT_I4 && actual.lVal == (IME_CMODE_NATIVE | IME_CMODE_ROMAN), "production setter reopens native conversion for next session");
    VariantClear(&actual);
    Check(SUCCEEDED(open->GetValue(&actual)) && actual.vt == VT_I4 && actual.lVal == 1, "production setter reopens IME");
    VariantClear(&actual);
    open->Release(); conversion->Release(); compartments->Release();
    manager->Deactivate(); manager->Release(); CoUninitialize();
    std::cout << checks << " real isolated TSF mode checks passed (no Sogou or game validation)\n";
}
