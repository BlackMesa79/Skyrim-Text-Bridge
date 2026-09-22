#include "ImePanel.h"
#include "Settings.h"
#include <algorithm>
#include <vector>
namespace TextBridge::ImePanel {
namespace {
HWND panel{}; std::wstring content; bool brief{}; float scale = 1.0f;
bool skipText=false; HDC textMask{};
COLORREF Color(const std::array<float,3>& c) { return RGB(int(c[0]*255),int(c[1]*255),int(c[2]*255)); }
constexpr int Width = 380;
int Px(int value) { return static_cast<int>(value * scale + .5f); }
std::vector<std::wstring> Lines() {
    std::vector<std::wstring> lines; size_t start = 0;
    for (;;) { auto end = content.find(L'\n', start); lines.push_back(content.substr(start, end - start)); if (end == std::wstring::npos) break; start = end + 1; }
    return lines;
}
int Height() { const auto lines = Lines(); return brief ? 62 : 108 + static_cast<int>(lines.size() > 2 ? lines.size() - 2 : 0) * 34; }
void Fill(HDC dc, RECT rect, COLORREF color) { auto brush = CreateSolidBrush(color); FillRect(dc, &rect, brush); DeleteObject(brush); }
void Label(HDC dc, std::wstring text, RECT rect, int size, COLORREF color, int weight = FW_NORMAL, UINT flags = DT_LEFT) {
    if (skipText) return;
    auto font = CreateFontW(-Px(size),0,0,0,weight,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,ANTIALIASED_QUALITY,0,L"Microsoft YaHei UI");
    auto old = SelectObject(dc, font); SetTextColor(dc, color); SetBkMode(dc, TRANSPARENT);
    DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &rect, flags | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    if (textMask) {
        auto prior=SelectObject(textMask,font); SetTextColor(textMask,RGB(255,255,255)); SetBkMode(textMask,TRANSPARENT);
        DrawTextW(textMask,text.c_str(),static_cast<int>(text.size()),&rect,flags | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        SelectObject(textMask,prior);
    }
    SelectObject(dc, old); DeleteObject(font);
}
RECT Box(int x, int y, int right, int bottom) { return {Px(x),Px(y),Px(right),Px(bottom)}; }
void Paint(HDC dc, RECT rect) {
    const auto settings=Settings::Get();
    const COLORREF background=Color(settings.background),gold=Color(settings.accent),white=Color(settings.foreground);
    constexpr COLORREF border=RGB(71,73,75),muted=RGB(147,153,158);
    Fill(dc, rect, background);
    auto pen = CreatePen(PS_SOLID, Px(1), border); auto oldPen = SelectObject(dc, pen); auto oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    RoundRect(dc, 0,0,rect.right,rect.bottom,Px(16),Px(16));
    SelectObject(dc, oldBrush); SelectObject(dc, oldPen); DeleteObject(pen);
    const auto lines = Lines();
    auto title = lines.front(); auto separator = title.find(L" ·"); if (separator != std::wstring::npos) title.resize(separator);
    Fill(dc, Box(18,25,23,30), brief ? muted : gold);
    Label(dc, title, Box(33,12,Width-100,44), 15, white, FW_MEDIUM);
    if (!brief) {
        Fill(dc, Box(Width-82,17,Width-18,39), RGB(43,47,51));
        Label(dc, Settings::KeyName(settings.hotkey)+L" 关闭", Box(Width-98,17,Width-18,39), 11, muted, FW_NORMAL, DT_CENTER);
        Fill(dc, Box(18,53,Width-18,54), RGB(52,55,58));
        if (lines.size() > 1) {
            const bool idle = lines[1] == L"请在当前输入框打字";
            Label(dc, idle ? L"就绪 · 在当前输入框开始输入" : lines[1], Box(20,63,Width-20,98), idle ? 14 : 21, idle ? muted : gold);
        }
        for (size_t i = 2; i < lines.size(); ++i) {
            auto line = lines[i]; const bool selected = line.starts_with(L"> ");
            const int y = 104 + static_cast<int>(i - 2) * 34;
            if (selected) { Fill(dc, Box(12,y,Width-12,y+32), RGB(57,54,45)); Fill(dc, Box(12,y+5,15,y+27), gold); }
            auto first = line.find_first_not_of(L" >"); if (first != std::wstring::npos) line.erase(0,first);
            auto dot = line.find(L". ");
            if (dot != std::wstring::npos && dot <= 2) {
                Label(dc, line.substr(0,dot), Box(24,y,49,y+32), 13, selected ? gold : muted);
                Label(dc, line.substr(dot+2), Box(57,y,Width-24,y+32), 17, selected ? gold : white, selected ? FW_MEDIUM : FW_NORMAL);
            } else Label(dc, line, Box(20,y,Width-20,y+32), 13, muted);
        }
    }
}
struct Surface {
    HDC dc{}; HBITMAP bitmap{}; HGDIOBJ old{}; unsigned char* pixels{};
    Surface(int w,int h) {
        BITMAPINFO info{}; info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); info.bmiHeader.biWidth=w; info.bmiHeader.biHeight=-h;
        info.bmiHeader.biPlanes=1; info.bmiHeader.biBitCount=32;
        dc=CreateCompatibleDC(nullptr); bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,reinterpret_cast<void**>(&pixels),nullptr,0);
        old=SelectObject(dc,bitmap); if (pixels) memset(pixels,0,w*h*4);
    }
    ~Surface() { SelectObject(dc,old); DeleteObject(bitmap); DeleteDC(dc); }
};
void RenderAlpha(Surface& result,int w,int h) {
    Surface base(w,h),mask(w,h);
    if (!result.pixels || !base.pixels || !mask.pixels) return;
    skipText=true; Paint(base.dc,{0,0,w,h}); skipText=false;
    textMask=mask.dc; Paint(result.dc,{0,0,w,h}); textMask=nullptr; GdiFlush();
    const float opacity=Settings::Get().opacity; const int radius=Px(8);
    for (int y=0;y<h;++y) for (int x=0;x<w;++x) {
        const int i=(y*w+x)*4; float coverage=mask.pixels[i]/255.f;
        float alpha=opacity+(1-opacity)*coverage;
        const int dx=std::max({radius-x,0,x-(w-1-radius)}),dy=std::max({radius-y,0,y-(h-1-radius)});
        if (dx*dx+dy*dy>radius*radius) alpha=0;
        for (int c=0;c<3;++c) result.pixels[i+c]=alpha==0?0:static_cast<unsigned char>(std::clamp(result.pixels[i+c]-base.pixels[i+c]*(1-opacity)*(1-coverage),0.f,255.f));
        result.pixels[i+3]=static_cast<unsigned char>(alpha*255+.5f);
    }
}
LRESULT CALLBACK Proc(HWND window, UINT message, WPARAM w, LPARAM l) {
    if (message == WM_NCHITTEST) return HTTRANSPARENT;
    if (message == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if (message == WM_ERASEBKGND) return 1;
    if (message == WM_TIMER) { ShowWindow(window, SW_HIDE); KillTimer(window, 1); return 0; }
    if (message == WM_PAINT) {
        PAINTSTRUCT paint{}; auto dc = BeginPaint(window, &paint); RECT rect{}; GetClientRect(window, &rect);
        auto buffer = CreateCompatibleDC(dc); auto bitmap = CreateCompatibleBitmap(dc, rect.right, rect.bottom); auto old = SelectObject(buffer, bitmap);
        Paint(buffer, rect); BitBlt(dc,0,0,rect.right,rect.bottom,buffer,0,0,SRCCOPY);
        SelectObject(buffer, old); DeleteObject(bitmap); DeleteDC(buffer); EndPaint(window, &paint); return 0;
    }
    if (message == WM_DESTROY) panel = nullptr;
    return DefWindowProcW(window, message, w, l);
}
}
void Show(HWND owner, const std::wstring& text, bool transient) {
    if (!IsWindow(panel)) {
        WNDCLASSW wc{}; wc.lpfnWndProc = Proc; wc.hInstance = GetModuleHandleW(nullptr); wc.lpszClassName = L"SkyrimTextBridge.Status.022"; RegisterClassW(&wc);
        panel = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT, wc.lpszClassName,L"",WS_POPUP,0,0,Width,100,owner,nullptr,wc.hInstance,nullptr);
    }
    if (!panel) return;
    content = text; brief = transient; KillTimer(panel, 1);
    RECT game{}; GetWindowRect(owner, &game);
    MONITORINFO monitor{sizeof(monitor)}; GetMonitorInfoW(MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST), &monitor);
    const auto settings=Settings::Get();
    scale = std::clamp(GetDpiForWindow(owner) / 96.0f, 1.0f, 2.5f)*settings.size;
    scale = std::min(scale, std::min((monitor.rcWork.right-monitor.rcWork.left) / float(Width), (monitor.rcWork.bottom-monitor.rcWork.top) / float(Height())));
    int width = Px(Width), height = Px(Height());
    int x=std::max(monitor.rcWork.left,std::min(game.left+LONG(std::max(0L,game.right-game.left-width)*settings.x),monitor.rcWork.right-width));
    int y=std::max(monitor.rcWork.top,std::min(game.top+LONG(std::max(0L,game.bottom-game.top-height)*settings.y),monitor.rcWork.bottom-height));
    Surface surface(width,height); RenderAlpha(surface,width,height);
    POINT destination{x,y},source{}; SIZE size{width,height}; BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
    UpdateLayeredWindow(panel,nullptr,&destination,&size,surface.dc,&source,0,&blend,ULW_ALPHA);
    SetWindowPos(panel,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    if (transient) SetTimer(panel,1,1400,nullptr);
}
void Hide() { if (IsWindow(panel)) { KillTimer(panel,1); ShowWindow(panel,SW_HIDE); } }
}
