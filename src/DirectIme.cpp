#include "DirectIme.h"
#include "PrismaBridge.h"
#include "BridgeCore.h"
#include "ImePanel.h"
#include "Settings.h"
#include <imm.h>
#include <commctrl.h>
#include <atomic>
#include <deque>
#include <mutex>
#include <algorithm>

namespace TextBridge::DirectIme {
namespace {
HWND hwnd{}; HIMC context{}, saved{};
UINT syncMessage{}, prismaAssociationMessage{};
constexpr UINT_PTR subclassId = 0x53544221;
std::atomic<bool> enabled{false}, attached{false}, composing{false}, posted{false}, subclassed{false};
std::atomic<HHOOK> bootstrap{};
std::atomic<bool> offTip{false};
std::atomic<bool> english{false};
bool appliedEnglish = false;
std::atomic<ULONGLONG> requestTime{0};
std::mutex mutex; std::deque<std::uint32_t> text;
char16_t highSurrogate{};
bool detaching = false;
#ifdef TEXTBRIDGE_IME_TEST
bool testForeground = true;
bool Foreground() { return testForeground; }
#else
bool Foreground() { return GetForegroundWindow() == hwnd; }
#endif
bool Allowed() { return enabled && Settings::Get().enabled && Foreground() && !Prisma::OwnsInput(); }
void Clear() { std::lock_guard lock(mutex); text.clear(); }
void Enqueue(std::u16string_view value) {
    auto decoded = DecodeUtf16(value); std::lock_guard lock(mutex);
    if (text.size() + decoded.size() <= MaxTextUnits) text.insert(text.end(), decoded.begin(), decoded.end());
}
std::wstring ReadComposition() {
    LONG size = ImmGetCompositionStringW(context, GCS_COMPSTR, nullptr, 0);
    if (size <= 0 || size > 8192 || size % 2) return {};
    std::wstring result(size / 2, L'\0');
    return ImmGetCompositionStringW(context, GCS_COMPSTR, result.data(), size) == size ? result : L"";
}
bool NativeMode() {
    DWORD conversion{}, sentence{};
    return ImmGetOpenStatus(context) && ImmGetConversionStatus(context, &conversion, &sentence) && (conversion & IME_CMODE_NATIVE);
}
std::wstring CandidateText() {
    std::wstring result;
    const DWORD size = ImmGetCandidateListW(context, 0, nullptr, 0);
    if (size < sizeof(CANDIDATELIST) || size > 1024 * 1024) return result;
    std::vector<DWORD> storage((size + 3) / 4);
    auto list = reinterpret_cast<CANDIDATELIST*>(storage.data());
    DWORD copied = ImmGetCandidateListW(context, 0, list, size);
    constexpr size_t offset = offsetof(CANDIDATELIST, dwOffset);
    if (copied < sizeof(CANDIDATELIST) || copied > size || list->dwCount > (copied - offset) / sizeof(DWORD)) return result;
    const auto start = std::min(list->dwPageStart, list->dwCount);
    const auto count = std::min<DWORD>(list->dwPageSize ? list->dwPageSize : 10, std::min<DWORD>(10, list->dwCount - start));
    for (DWORD i = start; i < start + count; ++i) {
        DWORD position = list->dwOffset[i];
        if (position < offset + list->dwCount * sizeof(DWORD) || position >= copied || position % 2) continue;
        auto chars = reinterpret_cast<const wchar_t*>(reinterpret_cast<const char*>(list) + position);
        size_t capacity = (copied - position) / 2, length = 0;
        while (length < capacity && chars[length]) ++length;
        if (length == capacity) continue;
        result += list->dwSelection == i ? L"\n> " : L"\n  ";
        result += std::to_wstring(i - start + 1) + L". " + std::wstring(chars, length);
    }
    return result;
}
void UpdatePanel() {
    auto composition = ReadComposition();
    composing = !composition.empty();
    std::wstring display = (english ? std::wstring(L"英文输入 · ") : std::wstring(L"中文输入 · ")) + Settings::KeyName(Settings::Get().hotkey) + L" 关闭";
    display += L"\n" + (composition.empty() ? std::wstring(L"请在当前输入框打字") : composition);
    auto candidates = CandidateText();
    if (composition.empty()) display += L"\nCtrl + Space 切换中英文";
    if (!composition.empty() && candidates.empty()) display += L"\n输入法未提供候选列表";
    ImePanel::Show(hwnd, display + candidates);
}
LRESULT CALLBACK Subclass(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
void Detach(bool tip = false) {
    if (detaching) return;
    detaching = true;
    posted = false;
    enabled = false; composing = false; highSurrogate = 0; Clear();
    english = false; appliedEnglish = false;
    const bool owned = attached.exchange(false);
    if (owned) {
        auto current = ImmGetContext(hwnd); if (current) ImmReleaseContext(hwnd, current);
        if (current == context) {
            ImmNotifyIME(context, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
            ImmAssociateContext(hwnd, saved);
        }
        saved = nullptr;
    }
    if (subclassed.exchange(false)) RemoveWindowSubclass(hwnd, Subclass, subclassId);
    if (context) { ImmDestroyContext(context); context = nullptr; }
    if (tip && Foreground() && !Prisma::OwnsInput()) ImePanel::Show(hwnd, L"输入法已关闭", true);
    else ImePanel::Hide();
    detaching = false;
}
void Sync() {
    if (!Allowed()) { Detach(offTip.exchange(false)); return; }
    if (!attached) {
        context = ImmCreateContext();
        if (!context) { Detach(); ImePanel::Show(hwnd, L"输入法启动失败", true); return; }
        attached = true;
        saved = ImmAssociateContext(hwnd, context);
        appliedEnglish = english.load();
        ImmSetOpenStatus(context, !appliedEnglish);
        UpdatePanel();
    }
    if (appliedEnglish != english.load()) {
        ImmNotifyIME(context, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
        composing = false; highSurrogate = 0;
        appliedEnglish = english.load();
        ImmSetOpenStatus(context, !appliedEnglish);
        UpdatePanel();
    }
}
LRESULT CALLBACK Subclass(HWND window, UINT message, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR) {
    // IMM cancellation may synchronously re-enter this callback during teardown.
    if (detaching) return DefSubclassProc(window, message, w, l);
    if (message == prismaAssociationMessage) { Prisma::Relinquish(); Detach(); return DefSubclassProc(window, message, w, l); }
    if (message == syncMessage) { posted = false; Sync(); return 0; }
    if (!Allowed() || message == WM_KILLFOCUS || message == WM_NCDESTROY) {
        Detach(); return DefSubclassProc(window, message, w, l);
    }
    if (attached) {
        if (english && message == WM_KEYDOWN) {
            // Translate here: Skyrim need not call TranslateMessage. Ignore its
            // WM_CHAR copy below, so each physical/repeated key is committed once.
            BYTE keys[256]{};
            if (GetKeyboardState(keys) && !(keys[VK_CONTROL] & 0x80) && !(keys[VK_MENU] & 0x80)) {
                wchar_t chars[16]{};
                const int count = ToUnicodeEx(static_cast<UINT>(w), (static_cast<unsigned long long>(l) >> 16) & 0xff,
                    keys, chars, 16, 0, GetKeyboardLayout(0));
                std::u16string printable;
                for (int i=0; i<count; ++i)
                    if (chars[i] >= 0x20 && chars[i] != 0x7f) printable.push_back(static_cast<char16_t>(chars[i]));
                if (!printable.empty()) Enqueue(printable);
            }
        }
        if (message == WM_IME_SETCONTEXT) return DefWindowProcW(window, message, w, 0);
        if (message == WM_IME_STARTCOMPOSITION) { composing = true; UpdatePanel(); composing = true; return 0; }
        if (message == WM_IME_ENDCOMPOSITION) { composing = false; UpdatePanel(); return 0; }
        if (message == WM_IME_COMPOSITION) {
            if (l & GCS_RESULTSTR) {
                const LONG bytes = ImmGetCompositionStringW(context, GCS_RESULTSTR, nullptr, 0);
                if (bytes > 0 && bytes <= MaxTextUnits * 2 && bytes % 2 == 0) {
                    std::u16string value(bytes / 2, u'\0');
                    if (ImmGetCompositionStringW(context, GCS_RESULTSTR, value.data(), bytes) == bytes) Enqueue(value);
                }
            }
            UpdatePanel(); return 0;
        }
        if (message == WM_IME_NOTIFY) { UpdatePanel(); return 0; }
        if (message == WM_IME_CHAR) return 0;
        if (message == WM_CHAR) {
            // In direct mode the Windows stream owns characters, including English.
            if (!english && !composing && !NativeMode() && w >= 0x20 && w != 0x7f) {
                char16_t c = static_cast<char16_t>(w);
                if (c >= 0xd800 && c <= 0xdbff) highSurrogate = c;
                else {
                    char16_t value[2]{highSurrogate, c};
                    Enqueue(highSurrogate ? std::u16string_view(value, 2) : std::u16string_view(&c, 1)); highSurrogate = 0;
                }
            }
            return 0;
        }
    }
    return DefSubclassProc(window, message, w, l);
}
void AttachSubclass() {
    if (!Allowed()) { enabled = false; posted = false; return; }
    subclassed = SetWindowSubclass(hwnd, Subclass, subclassId, 0) != FALSE;
    if (!subclassed) { enabled = false; posted = false; return; }
    Sync();
}
LRESULT CALLBACK Bootstrap(int code, WPARAM w, LPARAM l) {
    if (code >= 0) {
        const auto message = reinterpret_cast<MSG*>(l);
        if (message->hwnd == hwnd && message->message == syncMessage && w == PM_REMOVE) {
            AttachSubclass();
            if (auto hook = bootstrap.exchange(nullptr)) UnhookWindowsHookEx(hook);
        }
    }
    return CallNextHookEx(nullptr, code, w, l);
}
}
bool Install(HWND window) {
    hwnd = window;
    syncMessage = RegisterWindowMessageW(L"SkyrimTextBridge.DirectIme.Sync.v21");
    prismaAssociationMessage = RegisterWindowMessageW(L"PrismaUI.ImeAssociation");
    // No window hook or IME context until explicitly enabled.
    return IsWindow(hwnd) && syncMessage && prismaAssociationMessage;
}
bool Toggle() {
    if (Prisma::OwnsInput()) return false;
    enabled = !enabled.load();
    offTip = !enabled;
    if (!enabled && GetCurrentThreadId() == GetWindowThreadProcessId(hwnd, nullptr)) Detach(true);
    Tick(); return enabled;
}
void Reset() { enabled = false; offTip = false; Clear(); Tick(); }
bool Active() { return enabled || attached || subclassed || bootstrap.load(); }
void Tick() {
    if (!hwnd || !Active()) return;
    if (!subclassed && enabled && !bootstrap.load()) {
        if (GetCurrentThreadId() == GetWindowThreadProcessId(hwnd, nullptr)) AttachSubclass();
        else {
            HMODULE module{};
            GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(&Bootstrap), &module);
            bootstrap = SetWindowsHookExW(WH_GETMESSAGE, Bootstrap, module, GetWindowThreadProcessId(hwnd, nullptr));
            requestTime = GetTickCount64();
            if (!bootstrap.load()) { enabled = false; return; }
        }
    }
    if (bootstrap.load() && GetTickCount64() - requestTime > 3000) {
        if (auto hook = bootstrap.exchange(nullptr)) UnhookWindowsHookEx(hook);
        enabled = false; posted = false; return;
    }
    if ((subclassed || bootstrap.load()) && !posted.exchange(true) && !PostMessageW(hwnd, syncMessage, 0, 0)) posted = false;
}
bool SuppressKeyboard() { return enabled && !Prisma::OwnsInput(); }
bool BlockNavigation() { return composing; }
void ToggleEnglish() { if (enabled) { english = !english.load(); Tick(); } }
std::vector<std::uint32_t> Take(unsigned capacity) {
    std::lock_guard lock(mutex); std::vector<std::uint32_t> result;
    if (!Allowed()) { text.clear(); return result; }
    while (capacity-- && !text.empty()) { result.push_back(text.front()); text.pop_front(); }
    return result;
}
}
