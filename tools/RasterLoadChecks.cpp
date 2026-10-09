#include <Windows.h>
#include <thread>
#include <iostream>
#include <cstdlib>
int wmain(int argc, wchar_t** argv) {
    if (argc != 2) return 2;
    unsigned checks{};
    auto check = [&](bool ok, const char* name) {
        ++checks; if (!ok) { std::cerr << "FAIL " << name << '\n'; std::exit(1); }
    };
    // A DLL, not just an executable test: MSVC dynamic TLS initialization must
    // remain dormant during real LoadLibrary and every later thread attach.
    auto library = LoadLibraryW(argv[1]);
    check(library != nullptr, "normal DLL loading completes");
    auto count = reinterpret_cast<unsigned(*)()>(GetProcAddress(library, "RasterInitializationCount"));
    auto draw = reinterpret_cast<BOOL(*)()>(GetProcAddress(library, "RasterDrawProbe"));
    check(count && draw, "lifecycle probe exports available");
    check(count() == 0, "DLL load never constructs font/COM/DirectX renderer");
    for (unsigned i = 0; i < 8; ++i) { std::thread worker([] {}); worker.join(); }
    check(count() == 0, "new thread attach and exit never initialize renderer");
    check(draw() && count() == 1, "first actual glyph draw lazily initializes once");
    for (unsigned i = 0; i < 8; ++i) { std::thread worker([] {}); worker.join(); }
    check(count() == 1, "later threads do not create per-thread devices");
    check(draw() && count() == 1, "subsequent draw reuses private device");
    // Mimics CRT detach; no COM/device destructor is registered with the DLL.
    check(FreeLibrary(library) != FALSE, "DLL unload completes without graphics teardown");
    std::cout << checks << " real DLL load/thread/raster lifecycle checks passed\n";
}
