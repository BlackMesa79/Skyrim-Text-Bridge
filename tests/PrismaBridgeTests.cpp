#include <Windows.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
namespace SKSE::log { template<class... T> void info(const char*, T...) {} }
bool present=true, exported=true, supported=true, hasFocus=false;
unsigned requests=0;
HMODULE MockModule(LPCWSTR) { return present ? reinterpret_cast<HMODULE>(1) : nullptr; }
FARPROC MockExport(HMODULE,const char*);
#define GetModuleHandleW MockModule
#define GetProcAddress MockExport
#define TEXTBRIDGE_PRISMA_TEST
#include "../src/PrismaBridge.cpp"
#undef GetModuleHandleW
#undef GetProcAddress
using namespace PRISMA_UI_API;
struct FakeAPI final : IVPrismaUI1 {
    PrismaView CreateView(const char*,OnDomReadyCallback) noexcept override { return 0; }
    void Invoke(PrismaView,const char*,JSCallback) noexcept override {}
    void InteropCall(PrismaView,const char*,const char*) noexcept override {}
    void RegisterJSListener(PrismaView,const char*,JSListenerCallback) noexcept override {}
    bool HasFocus(PrismaView) noexcept override { return hasFocus; }
    bool Focus(PrismaView,bool,bool) noexcept override { return false; }
    void Unfocus(PrismaView) noexcept override {}
    void Show(PrismaView) noexcept override {}
    void Hide(PrismaView) noexcept override {}
    bool IsHidden(PrismaView) noexcept override { return false; }
    int GetScrollingPixelSize(PrismaView) noexcept override { return 0; }
    void SetScrollingPixelSize(PrismaView,int) noexcept override {}
    bool IsValid(PrismaView) noexcept override { return true; }
    void Destroy(PrismaView) noexcept override {}
    void SetOrder(PrismaView,int) noexcept override {}
    int GetOrder(PrismaView) noexcept override { return 0; }
    void CreateInspectorView(PrismaView) noexcept override {}
    void SetInspectorVisibility(PrismaView,bool) noexcept override {}
    bool IsInspectorVisible(PrismaView) noexcept override { return false; }
    void SetInspectorBounds(PrismaView,float,float,unsigned,unsigned) noexcept override {}
    bool HasAnyActiveFocus() noexcept override { return hasFocus; }
} fake;
void* Request(InterfaceVersion v) {
    ++requests;
    if(v!=InterfaceVersion::V1) std::abort();
    return supported ? &fake : nullptr;
}
FARPROC MockExport(HMODULE,const char* name) {
    // No release metadata exposed: compatible public V1 must still work.
    return exported && std::strcmp(name,"RequestPluginAPI")==0 ? reinterpret_cast<FARPROC>(&Request) : nullptr;
}
void Check(bool ok) { if(!ok) std::abort(); }
int main() {
    using namespace TextBridge::Prisma;
    Check(Install() && requests==1); Refresh(); Check(!OwnsInput());
    hasFocus=true; Refresh(); Check(OwnsInput());
    hasFocus=false; Refresh(); Check(!OwnsInput());
    Relinquish(); Check(OwnsInput()); Refresh(); Check(!OwnsInput());
    supported=false; Check(!Install()); Refresh(); Check(OwnsInput());
    exported=false; Check(!Install()); Refresh(); Check(OwnsInput());
    present=false; Check(Install()); Refresh(); Check(!OwnsInput());
    std::cout << "Prisma V1 negotiation, focus handoff, unsupported/missing API and absent framework checks passed\n";
}
