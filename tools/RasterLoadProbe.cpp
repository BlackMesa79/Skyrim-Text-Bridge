#include "PanelTextRaster.h"
#include <Windows.h>
extern "C" __declspec(dllexport) unsigned RasterInitializationCount() {
    return TextBridge::PanelTextRaster::TestInitializationCount();
}
extern "C" __declspec(dllexport) BOOL RasterDrawProbe() {
    return bool(TextBridge::PanelTextRaster::Render("😂 中文", 22, 160, 32, 0xffffffff));
}
