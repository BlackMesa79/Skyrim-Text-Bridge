#include "../src/ImePanel.cpp"
#include "../src/Settings.cpp"
#include <fstream>
void Render(const wchar_t* text, bool transient, const char* path) {
    using namespace TextBridge::ImePanel;
    content = text; brief = transient; scale = 1.5f;
    int width = Px(Width), height = Px(Height());
    BITMAPINFO info{}; info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); info.bmiHeader.biWidth = width; info.bmiHeader.biHeight = -height; info.bmiHeader.biPlanes = 1; info.bmiHeader.biBitCount = 32;
    void* bits{}; auto dc = CreateCompatibleDC(nullptr); auto bitmap = CreateDIBSection(dc,&info,DIB_RGB_COLORS,&bits,nullptr,0); auto old = SelectObject(dc,bitmap);
    Surface layered(width,height); RenderAlpha(layered,width,height);
    auto output=static_cast<unsigned char*>(bits);
    for (int y=0;y<height;++y) for (int x=0;x<width;++x) {
        const int i=(y*width+x)*4; const int bg=((x/40+y/40)%2)?95:65;
        for (int c=0;c<3;++c) output[i+c]=static_cast<unsigned char>(layered.pixels[i+c]+bg*(255-layered.pixels[i+3])/255);
        output[i+3]=255;
    }
    GdiFlush();
    BITMAPFILEHEADER header{}; header.bfType = 0x4d42; header.bfOffBits = sizeof(header)+sizeof(BITMAPINFOHEADER); header.bfSize = header.bfOffBits + width*height*4;
    std::ofstream file(path,std::ios::binary); file.write(reinterpret_cast<char*>(&header),sizeof(header)); file.write(reinterpret_cast<char*>(&info.bmiHeader),sizeof(BITMAPINFOHEADER)); file.write(static_cast<char*>(bits),width*height*4);
    SelectObject(dc,old); DeleteObject(bitmap); DeleteDC(dc);
}
int main() { auto settings=TextBridge::Settings::Data{}; settings.opacity=.4f; TextBridge::Settings::Set(settings);
    Render(L"中文输入 · F8 关闭\nshang gu juan zhou\n当前输入法：微软拼音\n  1. 上古卷轴\n> 2. 上古卷轴特别版\n  3. 上古卷轴重制版\n  4. 上古卷轴：天际",false,"build/preview/candidates.bmp");
    Render(L"英文输入 · F8 关闭\n请在当前输入框打字\n当前输入法：微软拼音\nCtrl + Space 切换中英文",false,"build/preview/ready.bmp");
    Render(L"输入法已关闭",true,"build/preview/off.bmp");
}
