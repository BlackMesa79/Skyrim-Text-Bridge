#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
int wmain(int argc,wchar_t** argv) {
    if(argc!=2)return 1;
    auto module=LoadLibraryExW(argv[1],nullptr,DONT_RESOLVE_DLL_REFERENCES);if(!module)return 2;
    if(!GetProcAddress(module,"SKSEPlugin_Query") || !GetProcAddress(module,"SKSEPlugin_Load"))return 3;
    const auto data=reinterpret_cast<const unsigned char*>(GetProcAddress(module,"SKSEPlugin_Version"));if(!data)return 4;
    // SDK PluginDeclaration: four-byte data version, compatibility at +0x304,
    // followed by four-byte flags and 16 packed versions (SDK static_assert layout).
    std::uint32_t record{};std::memcpy(&record,data,4);if(record!=1)return 5;
    std::uint32_t versions[16]{};std::memcpy(versions,data+0x30c,sizeof(versions));
    const std::uint32_t se=(1u<<24)|(5u<<16)|(97u<<4),ae=(1u<<24)|(6u<<16)|(1170u<<4);
    if(versions[0]!=se || versions[1]!=ae)return 6;
    for(int i=2;i<16;++i)if(versions[i])return 7;
    FreeLibrary(module);
    puts("DLL exports Query/Load; metadata lists only SE 1.5.97 and AE 1.6.1170 (entry point not executed)");
}
