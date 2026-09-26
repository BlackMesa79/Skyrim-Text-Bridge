#define TEXTBRIDGE_IME_TEST
#include "../src/DirectIme.cpp"
#include "../src/Settings.cpp"
#include <iostream>
#include <cstdlib>
#include <thread>
namespace { bool prisma = false; unsigned checks = 0, forwarded = 0; std::wstring panelText; LPARAM seenParam{}; }
namespace TextBridge::Prisma { bool OwnsInput() { return prisma; } void Relinquish() { prisma = true; } }
namespace TextBridge::ImePanel { void Show(HWND, const std::wstring& text, bool) { panelText = text; } void Hide() { panelText.clear(); } }
LRESULT CALLBACK Peer(HWND window, UINT message, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR) {
    if (message == WM_IME_NOTIFY) { ++forwarded; seenParam = l; return 713; }
    return DefSubclassProc(window, message, w, l);
}
void Check(bool ok, const char* label) { ++checks; if (!ok) { std::cerr << "FAIL: " << label << '\n'; std::exit(1); } }
int main() {
    using namespace TextBridge::DirectIme;
    WNDCLASSW wc{}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = GetModuleHandleW(nullptr); wc.lpszClassName = L"TextBridgeDirectImeTest";
    RegisterClassW(&wc);
    auto window = CreateWindowW(wc.lpszClassName,L"",WS_OVERLAPPED,0,0,200,100,nullptr,nullptr,wc.hInstance,nullptr);
    SetWindowSubclass(window, Peer, 99, 0);
    auto originalProc = GetWindowLongPtrW(window, GWLP_WNDPROC);
    Check(window && Install(window), "record target");
    Tick();
    Check(!Active() && !context && GetWindowLongPtrW(window, GWLP_WNDPROC) == originalProc, "idle never changes window procedure or IME");
    Check(SendMessageW(window, WM_IME_NOTIFY, 0, 123) == 713 && seenParam == 123, "idle forwards peer IME unchanged");
    auto original = ImmGetContext(window); if (original) ImmReleaseContext(window, original);
    Check(Toggle() && attached && context, "lazy attach on window thread");
    Check(SuppressKeyboard() && !composing, "suppress text before STARTCOMPOSITION");
    Check(!panelText.empty(), "visible enabled status requested");
    Check(panelText.find(L"当前输入法：测试输入法") != std::wstring::npos, "panel identifies selected IME");
    testInputMethodName=L"另一输入法";
    SendMessageW(window, WM_INPUTLANGCHANGE, 0, reinterpret_cast<LPARAM>(GetKeyboardLayout(0)));
    Check(panelText.find(L"当前输入法：另一输入法") != std::wstring::npos, "layout message refreshes name");
    testInputMethodName=L"同语言另一配置"; nameCheckedAt=GetTickCount64()-1000;
    SendMessageW(window, syncMessage, 0, 0);
    Check(panelText.find(L"同语言另一配置") != std::wstring::npos, "periodic refresh covers same-layout TSF switches");
    ImmSetOpenStatus(context, FALSE);
    SendMessageW(window, WM_CHAR, L'a', 0);
    auto ascii = Take(5); Check(ascii.size() == 1 && ascii[0] == 'a', "English comes only from Windows stream");
    ToggleEnglish(); SendMessageW(window, syncMessage, 0, 0);
    Check(english && !ImmGetOpenStatus(context) && panelText.find(L"英文输入") != std::wstring::npos, "explicit English mode closes IME and labels panel");
    Check(panelText.find(L"同语言另一配置") != std::wstring::npos, "English mode preserves selected IME identity");
    BYTE originalKeys[256]{}, cleanKeys[256]{}; GetKeyboardState(originalKeys); SetKeyboardState(cleanKeys);
    SendMessageW(window, WM_KEYDOWN, 'A', 0x001e0001);
    SendMessageW(window, WM_CHAR, L'a', 0);
    auto latin = Take(5); Check(latin.size()==1 && latin[0]=='a', "English key translates exactly once even with WM_CHAR copy");
    cleanKeys[VK_SHIFT]=0x80; SetKeyboardState(cleanKeys);
    SendMessageW(window, WM_KEYDOWN, 'A', 0x001e0001);
    latin=Take(5); Check(latin.size()==1 && latin[0]=='A', "English respects Shift");
    cleanKeys[VK_CONTROL]=0x80; SetKeyboardState(cleanKeys);
    SendMessageW(window, WM_KEYDOWN, VK_SPACE, 0x00390001);
    Check(Take(5).empty(), "language shortcut never inserts space");
    SetKeyboardState(originalKeys);
    ToggleEnglish(); SendMessageW(window, syncMessage, 0, 0);
    Check(!english && ImmGetOpenStatus(context), "switch back restores IME");
    ImmSetOpenStatus(context, TRUE);
    composing = true; SendMessageW(window, WM_CHAR, L'p', 0);
    Check(Take(5).empty(), "preedit character not committed");
    Enqueue(u"中文"); auto first = Take(1);
    Check(first.size() == 1 && first[0] == 0x4e2d, "bounded Unicode commit drain");
    prisma = true; SendMessageW(window, WM_NULL, 0, 0);
    auto restored = ImmGetContext(window); if (restored) ImmReleaseContext(window, restored);
    Check(!Active() && restored == original && text.empty(), "Prisma handoff fully unhooks and restores");
    Check(SendMessageW(window, WM_IME_NOTIFY, 0, 456) == 713 && seenParam == 456, "peer candidates still receive messages after handoff");
    prisma = false; Toggle();
    auto foreign = ImmCreateContext(); ImmAssociateContext(window, foreign);
    prisma = true; SendMessageW(window, WM_NULL, 0, 0);
    auto current = ImmGetContext(window); if (current) ImmReleaseContext(window, current);
    Check(current == foreign, "never replace foreign context");
    ImmAssociateContext(window, original); ImmDestroyContext(foreign);
    prisma = false; Toggle(); SendMessageW(window, WM_KILLFOCUS, 0, 0);
    Check(!Active() && panelText.empty(), "focus loss clears mode and panel");
    Toggle(); Toggle(); Check(!Active() && panelText == L"输入法已关闭", "toggle off gives visible status and unhooks");
    Toggle(); Reset(); SendMessageW(window, syncMessage, 0, 0);
    Check(!Active(), "menu/load reset");
    std::atomic<bool> requested{false}, finished{false};
    std::thread request([&] { TextBridge::DirectIme::Toggle(); requested = true; while (!finished) std::this_thread::yield(); });
    while (!requested) std::this_thread::yield();
    const auto deadline = GetTickCount64() + 2000;
    while (!attached && GetTickCount64() < deadline) {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&message); DispatchMessageW(&message); }
        if (!attached) MsgWaitForMultipleObjects(0, nullptr, FALSE, 20, QS_ALLINPUT);
    }
    finished = true; request.join();
    if (!attached) std::cerr << "activation state: enabled=" << enabled << " posted=" << posted << " hook=" << bootstrap.load() << " subclass=" << subclassed << '\n';
    Check(attached && !bootstrap.load(), "cross-thread activation installs on owner thread and removes bootstrap hook");
    Toggle();
    Check(!Active(), "cross-thread activation leaves no hook after disable");
    DestroyWindow(window); Check(!context, "no owned context remains");
    std::cout << checks << " direct IME checks passed (peer subclass and panel are test doubles)\n";
}
