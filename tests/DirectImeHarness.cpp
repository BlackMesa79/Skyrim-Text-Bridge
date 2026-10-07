#define TEXTBRIDGE_IME_TEST
#include "../src/DirectIme.cpp"
#include "../src/Settings.cpp"
#include "../src/InputClientApi.cpp"
#include <iostream>
#include <cstdlib>
#include <thread>
namespace { bool prisma = false; unsigned checks = 0, forwarded = 0, escapeForwarded = 0; std::wstring panelText; LPARAM seenParam{}; }
namespace TextBridge::Prisma { bool OwnsInput() { return prisma; } void Relinquish() { prisma = true; } }
namespace TextBridge::ImePanel { void Show(HWND, const std::wstring& text, bool) { panelText = text; } void Hide() { panelText.clear(); } }
LRESULT CALLBACK Peer(HWND window, UINT message, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR) {
    if (message == WM_IME_NOTIFY) { ++forwarded; seenParam = l; return 713; }
    if ((message == WM_KEYDOWN || message == WM_KEYUP) && w == VK_ESCAPE) ++escapeForwarded;
    return DefSubclassProc(window, message, w, l);
}
void Check(bool ok, const char* label) { ++checks; if (!ok) { std::cerr << "FAIL: " << label << '\n'; std::exit(1); } }
int main() {
    using namespace TextBridge::DirectIme;
    const DWORD native = IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_SYMBOL | IME_CMODE_ROMAN;
    Check(TextBridge::ImeLanguage::Conversion(native,true) == IME_CMODE_ROMAN, "English clears native/fullwidth/symbol while preserving unrelated conversion flags");
    Check(TextBridge::ImeLanguage::Conversion(IME_CMODE_ROMAN,false) == (IME_CMODE_NATIVE | IME_CMODE_ROMAN), "next native session can reopen after Sogou retained English mode");
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
    const auto inherited = original;
    original = ImmCreateContext();
    Check(original != nullptr, "create known game context for exit-state checks");
    ImmAssociateContext(window, original);
    Check(Toggle() && attached && context, "lazy attach on window thread");
    Check(tsfModeChanges == 1 && !tsfEnglish, "activation requests native mode for TSF-backed IME");
    SendMessageW(window, WM_KEYDOWN, VK_ESCAPE, 0);
    SendMessageW(window, WM_KEYUP, VK_ESCAPE, 0);
    Check(!escapeForwarded && SuppressKeyboard(), "Windows Esc pair stays local; game dispatcher owns action");
    Check(SuppressKeyboard() && !composing, "suppress text before STARTCOMPOSITION");
    std::uint32_t clientHotkey=0;
    Check(SkyrimTextBridge_QueryInputV1(1,&clientHotkey)==7 && clientHotkey==66, "native client sees actual hotkey and active state");
    Check(!SkyrimTextBridge_QueryInputV1(2,&clientHotkey) && !SkyrimTextBridge_QueryInputV1(1,nullptr), "native API rejects unsupported version and invalid output");
    Enqueue(u"字");
    Check(BlockNavigation() && (SkyrimTextBridge_QueryInputV1(1,&clientHotkey)&8), "pending committed text blocks confirming key before drain");
    Take(5);Check(!BlockNavigation(), "navigation restored after pending commit drain");
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
    Check(!Escape() && enabled && cancelComposition, "Esc cancels live composition without exiting immediately");
    SendMessageW(window, syncMessage, 0, 0);
    Check(!composing && !cancelComposition && enabled, "composition cancellation runs on window thread");
    Enqueue(u"中文"); auto first = Take(1);
    Check(first.size() == 1 && first[0] == 0x4e2d, "bounded Unicode commit drain");
    Check(!Escape() && enabled && !text.empty(), "Esc waits for committed text instead of discarding it");
    if (original) ImmSetOpenStatus(original, TRUE);
    auto changesBeforeHandoff = tsfModeChanges;
    prisma = true; SendMessageW(window, WM_NULL, 0, 0);
    auto restored = ImmGetContext(window); if (restored) ImmReleaseContext(window, restored);
    Check(!Active() && restored == original && text.empty(), "Prisma handoff fully unhooks and restores");
    Check(!original || ImmGetOpenStatus(original), "Prisma handoff preserves original context Chinese state");
    Check(tsfModeChanges == changesBeforeHandoff, "Prisma handoff never writes TSF mode");
    Check(SendMessageW(window, WM_IME_NOTIFY, 0, 456) == 713 && seenParam == 456, "peer candidates still receive messages after handoff");
    prisma = false; Toggle();
    auto foreign = ImmCreateContext(); ImmAssociateContext(window, foreign); ImmSetOpenStatus(foreign, TRUE);
    prisma = true; SendMessageW(window, WM_NULL, 0, 0);
    auto current = ImmGetContext(window); if (current) ImmReleaseContext(window, current);
    Check(current == foreign, "never replace foreign context");
    Check(ImmGetOpenStatus(foreign), "foreign context remains open after handoff");
    ImmAssociateContext(window, original); ImmDestroyContext(foreign);
    prisma = false; Toggle();
    foreign = ImmCreateContext(); ImmAssociateContext(window, foreign); ImmSetOpenStatus(foreign, TRUE);
    changesBeforeHandoff = tsfModeChanges;
    Reset(); SendMessageW(window, syncMessage, 0, 0);
    current = ImmGetContext(window); if (current) ImmReleaseContext(window, current);
    Check(!Active() && current == foreign && ImmGetOpenStatus(foreign), "ordinary reset does not close or replace a foreign context");
    Check(tsfModeChanges == changesBeforeHandoff, "foreign context reset never writes thread conversion mode");
    ImmAssociateContext(window, original); ImmDestroyContext(foreign);
    prisma = false; Toggle(); SendMessageW(window, WM_KILLFOCUS, 0, 0);
    Check(!Active() && panelText.empty(), "focus loss clears mode and panel");
    Toggle(); Toggle(); Check(!Active() && panelText == L"输入法已关闭", "toggle off gives visible status and unhooks");
    if (original) ImmSetOpenStatus(original, TRUE);
    Toggle();
    Check(ImmGetOpenStatus(context), "next session reopens selected IME without layout switching");
    Check(Escape() && !Active() && panelText == L"输入法已关闭", "Esc after input exits fully with visible status");
    Check(tsfEnglish, "completed Esc requests TSF English mode as well as IMM close");
    restored = ImmGetContext(window); if (restored) ImmReleaseContext(window, restored);
    Check(restored == original && (!original || !ImmGetOpenStatus(original)), "gameplay restores original association with IME closed");
    Check(!SuppressKeyboard() && !BlockNavigation() && !Escape(), "exited mode releases typing and idle Esc passes through");
    Check(SendMessageW(window, WM_IME_NOTIFY, 0, 789) == 713 && seenParam == 789, "normal exit leaves no subclass intercepting peer messages");
    SendMessageW(window, WM_KEYDOWN, VK_ESCAPE, 0);
    SendMessageW(window, WM_KEYUP, VK_ESCAPE, 0);
    Check(escapeForwarded == 2, "next ordinary Windows Esc pair works after exit");
    Toggle(); auto beforeBackground = tsfModeChanges;
    testForeground = false; SendMessageW(window, WM_KILLFOCUS, 0, 0);
    Check(!Active() && tsfModeChanges == beforeBackground, "Alt+Tab never changes another app's TSF mode");
    testForeground = true;
    Toggle(); SkyrimTextBridge_ResetInputV1(); SendMessageW(window, syncMessage, 0, 0);
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
    ImmAssociateContext(window, inherited); ImmDestroyContext(original);
    DestroyWindow(window); Check(!context, "no owned context remains");
    std::cout << checks << " direct IME checks passed (peer subclass and panel are test doubles)\n";
}
