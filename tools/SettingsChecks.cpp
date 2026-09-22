#include "../src/Settings.cpp"
#include "../src/ImePanel.cpp"
#include <iostream>
#include <limits>
#include <filesystem>
int main() {
    using namespace TextBridge;
    auto path=std::filesystem::absolute("build/preview/settings-test.ini").wstring();
    Settings::Load(path); auto value=Settings::Data{};
    value.enabled=false; value.hotkey=65; value.x=.8f; value.y=.3f; value.opacity=.25f; value.background={.2f,.3f,.4f};
    Settings::Set(value); if (!Settings::Save()) return 1;
    Settings::Set({}); Settings::Load(path); auto loaded=Settings::Get();
    if (loaded.enabled || loaded.hotkey!=65 || std::abs(loaded.opacity-.25f)>.0001f || loaded.background!=value.background) return 2;
    value.size=std::numeric_limits<float>::quiet_NaN(); value.opacity=2; value.x=-3;
    Settings::Set(value); auto clamped=Settings::Get(); if (clamped.size!=1 || clamped.opacity!=1 || clamped.x!=0) return 3;
    ImePanel::content=L"输入法已开启 · F7 关闭\nzhong wen\n> 1. 中文\n  2. 中问"; ImePanel::brief=false; ImePanel::scale=1;
    const int width=ImePanel::Width,height=ImePanel::Height();
    for (auto opacity : {0.f,.5f,1.f}) {
        value=Settings::Data{}; value.opacity=opacity; Settings::Set(value);
        ImePanel::Surface surface(width,height); ImePanel::RenderAlpha(surface,width,height);
        bool solidText=false;
        for (int i=0;i<width*height;++i) {
            auto pixel=surface.pixels+i*4;
            if (pixel[3]==255) solidText=true;
            if (pixel[0]>pixel[3]+1 || pixel[1]>pixel[3]+1 || pixel[2]>pixel[3]+1) return 4;
        }
        if (!solidText || surface.pixels[3]!=0) return 5;
        const int background=(height/2*width+5)*4+3;
        if (std::abs(int(surface.pixels[background])-int(opacity*255+.5f))>1) return 6;
    }
    std::cout << "Settings persistence, invalid-value clamps, 0/50/100% background alpha and solid text checks passed\n";
}
