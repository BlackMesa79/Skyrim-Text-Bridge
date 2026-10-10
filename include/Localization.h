#pragma once
#include "Settings.h"
#include <Windows.h>
#include <array>
#include <string>
#include <string_view>

namespace TextBridge::Localization {
enum class Language : unsigned { English, Chinese, Japanese, Korean };
enum class Key : unsigned {
    Settings, LanguageLabel, AutoLanguage, Enabled, AutoEnable, AutoHint, ControlsHint,
    FontHint, FontMissing, Hotkey, ChangeHotkey, CaptureHotkey, Layout, X, Y, Scale,
    Background, Accent, TextColor, Opacity, Save, Reset, Applied, Saved, SaveFailed,
    ResetDone, EnglishInput, ChineseInput, JapaneseInput, KoreanInput, NativeInput,
    Close, Idle, Ready, ModeHint, NoCandidates, Disabled, StartFailed, ImeName,
    KeyboardLanguage, Unknown, Count
};
// English, Simplified Chinese, Japanese, Korean. UI language never changes the
// active keyboard/IME profile. Static strings are safe across window/HUD threads.
inline constexpr std::array<std::array<const wchar_t*, 4>, static_cast<unsigned>(Key::Count)> messages{{
    {L"Input & panel", L"输入与面板", L"入力とパネル", L"입력 및 패널"},
    {L"Interface language", L"界面语言", L"表示言語", L"인터페이스 언어"},
    {L"Auto (Windows)", L"自动（Windows）", L"自動（Windows）", L"자동 (Windows)"},
    {L"Enable mod", L"启用模组", L"MOD を有効化", L"모드 활성화"},
    {L"Activate on text focus", L"文本框聚焦时自动开启", L"入力欄のフォーカスで自動開始", L"입력란 포커스 시 자동 시작"},
    {L"Auto: SkyUI, RaceMenu, Meridian. Manual: console, Modex. Prisma uses native input.", L"自动：SkyUI、RaceMenu、Meridian。手动：控制台、Modex。Prisma 使用原生输入。", L"自動：SkyUI・RaceMenu・Meridian。手動：コンソール・Modex。Prisma は標準入力。", L"자동: SkyUI, RaceMenu, Meridian. 수동: 콘솔, Modex. Prisma는 자체 입력 사용."},
    {L"Toggle key: on/off. Esc: cancel/exit. Use the IME's own language key.", L"快捷键开关；Esc 取消或退出；使用输入法自身的语言切换键。", L"設定キー：開始/終了。Esc：取消/終了。言語切替は IME のキーを使用。", L"단축키: 시작/종료. Esc: 취소/종료. 언어 전환은 입력기 키를 사용하세요."},
    {L"Font: enable the needed language in Menu Framework and use a matching font.", L"字体：在 Menu Framework 启用所需语言，并使用支持该语言的字体。", L"フォント：Menu Framework で必要な言語を有効にし、対応フォントを使用。", L"글꼴: Menu Framework에서 필요한 언어를 활성화하고 지원 글꼴을 사용하세요."},
    {L"The framework font lacks glyphs for this language. Check its font settings.", L"框架字体缺少当前语言字形，请检查字体设置。", L"この言語の文字が不足しています。フレームワークのフォント設定を確認。", L"현재 언어의 글자가 없습니다. 프레임워크 글꼴 설정을 확인하세요."},
    {L"Input key: ", L"输入快捷键：", L"入力キー：", L"입력 단축키: "},
    {L"Change key", L"修改快捷键", L"キーを変更", L"단축키 변경"},
    {L"Press a key (Esc cancels)", L"等待按键（Esc 取消）", L"キーを押す（Esc で取消）", L"키를 누르세요 (Esc 취소)"},
    {L"Panel layout", L"面板布局", L"パネル配置", L"패널 배치"},
    {L"Horizontal position", L"水平位置", L"水平位置", L"가로 위치"},
    {L"Vertical position", L"垂直位置", L"垂直位置", L"세로 위치"},
    {L"Panel scale", L"面板缩放", L"パネル倍率", L"패널 크기"},
    {L"Background color", L"背景颜色", L"背景色", L"배경 색상"},
    {L"Accent color", L"强调颜色", L"アクセント色", L"강조 색상"},
    {L"Text color", L"文字颜色", L"文字色", L"글자 색상"},
    {L"Background opacity", L"背景不透明度", L"背景の不透明度", L"배경 불투명도"},
    {L"Save settings", L"保存设置", L"設定を保存", L"설정 저장"},
    {L"Reset defaults", L"恢复默认", L"初期設定に戻す", L"기본값 복원"},
    {L"Applied. Save to keep changes.", L"已应用，保存后下次生效。", L"適用済み。次回も使うには保存。", L"적용됨. 유지하려면 저장하세요."},
    {L"Settings saved.", L"设置已保存。", L"設定を保存しました。", L"설정을 저장했습니다."},
    {L"Save failed. Check file permissions.", L"保存失败，请检查文件权限。", L"保存失敗。ファイルの権限を確認。", L"저장 실패. 파일 권한을 확인하세요."},
    {L"Defaults restored. Save to keep them.", L"已恢复默认，保存后保留。", L"初期設定に戻しました。保存で保持。", L"기본값 복원됨. 유지하려면 저장하세요."},
    {L"English input", L"英文输入", L"英字入力", L"영문 입력"},
    {L"Chinese input", L"中文输入", L"中国語入力", L"중국어 입력"},
    {L"Japanese input", L"日语输入", L"日本語入力", L"일본어 입력"},
    {L"Korean input", L"韩语输入", L"韓国語入力", L"한국어 입력"},
    {L"IME input", L"输入法模式", L"IME 入力", L"입력기 모드"},
    {L"Close", L"关闭", L"終了", L"종료"},
    {L"Type in the focused field", L"请在当前输入框打字", L"入力欄に文字を入力", L"현재 입력란에 입력하세요"},
    {L"Ready · Type here", L"就绪 · 开始输入", L"準備完了 · 入力できます", L"준비됨 · 입력하세요"},
    {L"Follows IME mode · Esc exits", L"跟随输入法状态 · Esc 退出", L"IME のモードに連動 · Esc で終了", L"입력기 상태 따름 · Esc 종료"},
    {L"No candidate list from this IME", L"输入法未提供候选列表", L"IME から候補一覧がありません", L"입력기가 후보 목록을 제공하지 않음"},
    {L"Input mode closed", L"输入法已关闭", L"入力モード終了", L"입력 모드 종료"},
    {L"Unable to start input mode", L"输入法启动失败", L"入力モードを開始できません", L"입력 모드를 시작할 수 없음"},
    {L"Input method: ", L"当前输入法：", L"入力方式：", L"입력기: "},
    {L"Keyboard language: ", L"键盘语言：", L"キーボード言語：", L"키보드 언어: "},
    {L"Unknown", L"未知", L"不明", L"알 수 없음"}
}};
inline Language FromWindows(LANGID id) {
    switch (PRIMARYLANGID(id)) {
    case LANG_CHINESE: return Language::Chinese;
    case LANG_JAPANESE: return Language::Japanese;
    case LANG_KOREAN: return Language::Korean;
    default: return Language::English;
    }
}
inline Language Resolve(int setting, LANGID windows) {
    return setting >= 1 && setting <= 4 ? static_cast<Language>(setting - 1) : FromWindows(windows);
}
inline Language Current() { return Resolve(Settings::Get().language, GetUserDefaultUILanguage()); }
inline const wchar_t* Wide(Key key, Language language) { return messages.at(static_cast<unsigned>(key)).at(static_cast<unsigned>(language)); }
inline const wchar_t* Wide(Key key) { return Wide(key, Current()); }
inline std::string Utf8(std::wstring_view text) {
    const int length = static_cast<int>(text.size());
    const auto size = WideCharToMultiByte(CP_UTF8, 0, text.data(), length, nullptr, 0, nullptr, nullptr);
    std::string result(size, 0);
    if (size) WideCharToMultiByte(CP_UTF8, 0, text.data(), length, result.data(), size, nullptr, nullptr);
    return result;
}
inline std::string Text(Key key, Language language) { return Utf8(Wide(key, language)); }
inline std::string Text(Key key) { return Text(key, Current()); }
inline Key InputTitle(bool english, LANGID keyboard) {
    if (english) return Key::EnglishInput;
    switch (PRIMARYLANGID(keyboard)) {
    case LANG_CHINESE: return Key::ChineseInput;
    case LANG_JAPANESE: return Key::JapaneseInput;
    case LANG_KOREAN: return Key::KoreanInput;
    default: return Key::NativeInput;
    }
}
}
