#include "Localization.h"
#include "ImeLanguage.h"
#include "../src/Settings.cpp"
#include <iostream>
#include <stdexcept>
int main() {
    using namespace TextBridge;
    using namespace Localization;
    unsigned checks{};
    auto check = [&](bool ok, const char* name) { if (!ok) throw std::runtime_error(name); ++checks; };
    const auto zh = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
    const auto ja = MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
    const auto ko = MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT);
    check(Resolve(0,zh)==Language::Chinese && Resolve(0,ja)==Language::Japanese && Resolve(0,ko)==Language::Korean,
          "auto resolves Windows display language for CJK");
    check(Resolve(0,MAKELANGID(LANG_FRENCH,SUBLANG_DEFAULT))==Language::English, "unsupported interface language falls back to English");
    check(Resolve(1,ja)==Language::English && Resolve(4,zh)==Language::Korean && Resolve(99,ja)==Language::Japanese,
          "explicit preference overrides display language with safe invalid fallback");
    for (unsigned lang=0;lang<4;++lang) for (unsigned key=0;key<static_cast<unsigned>(Key::Count);++key) {
        const auto wide=Wide(static_cast<Key>(key),static_cast<Language>(lang));
        auto utf8=Text(static_cast<Key>(key),static_cast<Language>(lang));
        std::wstring decoded(wcslen(wide),0);
        const auto length=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,utf8.data(),static_cast<int>(utf8.size()),decoded.data(),static_cast<int>(decoded.size()));
        check(!utf8.empty() && length==decoded.size() && decoded==wide, "every locale has valid complete UTF8 text");
    }
    check(InputTitle(false,zh)==Key::ChineseInput && InputTitle(false,ja)==Key::JapaneseInput && InputTitle(false,ko)==Key::KoreanInput,
          "IME title follows keyboard rather than interface language");
    check(InputTitle(true,ja)==Key::EnglishInput && InputTitle(false,0)==Key::NativeInput, "Latin and unknown IME title safe");
    check(ImeLanguage::NativeState(ko,false,IME_CMODE_NATIVE)==true, "Korean conversion wins over unreliable closed state");
    check(ImeLanguage::NativeState(ko,true,0)==false, "Korean Latin conversion stays English even with open flag");
    check(ImeLanguage::NativeState(ja,true,{})==true && ImeLanguage::NativeState(ja,false,{})==false,
          "Japanese open-only state supported");
    check(ImeLanguage::NativeState(ja,false,IME_CMODE_NATIVE)==false, "Japanese closed state overrides stale conversion");
    check(ImeLanguage::NativeState(ja,true,IME_CMODE_NATIVE|IME_CMODE_KATAKANA|IME_CMODE_FULLSHAPE)==true,
          "Japanese kana and fullwidth are native");
    check(ImeLanguage::NativeState(zh,true,IME_CMODE_NATIVE)==true && ImeLanguage::NativeState(zh,false,IME_CMODE_NATIVE)==false,
          "Chinese open/closed behavior retained");
    check(!ImeLanguage::NativeState(zh,{},{}).has_value() && !ImeLanguage::NativeState(ko,false,{}).value_or(false),
          "unknown conversion is not guessed native");
    std::cout << checks << " localization and CJK mode policy checks passed\n";
}
