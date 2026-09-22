#include "Settings.h"
#include <Windows.h>
#include <mutex>
#include <algorithm>
#include <cmath>
namespace TextBridge::Settings {
namespace { Data current; std::mutex mutex; std::wstring file; }
Data Get() { std::lock_guard lock(mutex); return current; }
void Set(Data value) {
    auto limit = [](float v, float lo, float hi, float fallback) { return std::isfinite(v) ? std::clamp(v,lo,hi) : fallback; };
    value.hotkey = std::clamp(value.hotkey,2,255);
    value.x=limit(value.x,0,1,.025f); value.y=limit(value.y,0,1,.065f);
    value.size=limit(value.size,.6f,1.8f,1); value.opacity=limit(value.opacity,0,1,.92f);
    for (auto color : {&value.background,&value.accent,&value.foreground}) for (auto& c : *color) c=limit(c,0,1,.5f);
    std::lock_guard lock(mutex); current=value;
}
void Load(const std::wstring& path) {
    file=path; Data value;
    auto read = [&](const wchar_t* section,const wchar_t* key,float fallback) {
        wchar_t buffer[64]{}; GetPrivateProfileStringW(section,key,L"",buffer,64,file.c_str());
        try { return std::stof(buffer); } catch (...) { return fallback; }
    };
    value.enabled=read(L"Input",L"Enabled",1)!=0;
    value.hotkey=GetPrivateProfileIntW(L"Input",L"HotkeyScanCode",66,file.c_str());
    value.x=read(L"Panel",L"X",value.x); value.y=read(L"Panel",L"Y",value.y);
    value.size=read(L"Panel",L"Scale",1); value.opacity=read(L"Panel",L"BackgroundOpacity",value.opacity);
    for (auto item : {std::pair{L"Background",&value.background},std::pair{L"Accent",&value.accent},std::pair{L"Text",&value.foreground}})
        for (int i=0;i<3;++i) (*item.second)[i]=read(item.first,i==0?L"R":i==1?L"G":L"B",(*item.second)[i]);
    Set(value);
}
bool Save() {
    const auto value=Get(); bool ok=true;
    auto write = [&](const wchar_t* section,const wchar_t* key,float v) { ok = WritePrivateProfileStringW(section,key,std::to_wstring(v).c_str(),file.c_str()) && ok; };
    write(L"Input",L"Enabled",value.enabled); // Key is integer for GetPrivateProfileInt.
    ok = WritePrivateProfileStringW(L"Input",L"HotkeyScanCode",std::to_wstring(value.hotkey).c_str(),file.c_str()) && ok;
    write(L"Panel",L"X",value.x); write(L"Panel",L"Y",value.y); write(L"Panel",L"Scale",value.size); write(L"Panel",L"BackgroundOpacity",value.opacity);
    for (auto item : {std::pair{L"Background",&value.background},std::pair{L"Accent",&value.accent},std::pair{L"Text",&value.foreground}})
        for (int i=0;i<3;++i) write(item.first,i==0?L"R":i==1?L"G":L"B",(*item.second)[i]);
    return ok;
}
std::wstring KeyName(int scan) {
    wchar_t buffer[64]{};
    LONG code=((scan & 0x7f)<<16) | ((scan & 0x80) ? (1<<24):0);
    if (GetKeyNameTextW(code,buffer,64)) return buffer;
    return L"Key " + std::to_wstring(scan);
}
}
