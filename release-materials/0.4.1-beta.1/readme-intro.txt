SKYRIM TEXT BRIDGE 0.4.1-beta.1
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

TEST BUILD / 测试版
Shorter settings help; English, Simplified Chinese, Japanese and Korean interfaces; initial Japanese/Korean IME handling. Offline checks pass, but actual Japanese/Korean engines need game testing. Stable 0.4.0 remains available. Emoji display is unsupported.
精简设置说明，加入英、简中、日、韩界面与初步日/韩输入处理。离线检查通过，实际日/韩输入法仍需游戏测试，保留稳定版 0.4.0。不支持 Emoji 显示。

REQUIREMENTS / 前置
Matching SKSE64 and Address Library; Media Keys Fix SKSE or a working equivalent nonexclusive keyboard patch; SKSE Menu Framework with the required HUD/ImGui exports and suitable fonts. Exact admitted runtimes: 1.5.97, 1.6.1170, 1.7.99 and 1.7.104. Every prerequisite must independently support your runtime. 1.7.104 uses SKSE 2.3.1; Media Keys Fix 1.0.2 documents 1.7.99, and its 1.7.104 operation needs confirmation.
需要匹配的 SKSE64 和 Address Library、Media Keys Fix SKSE 或有效等效非独占键盘补丁，以及有相应 HUD/ImGui 导出与字体的 SKSE Menu Framework。精确准入：1.5.97、1.6.1170、1.7.99、1.7.104，各前置需独立支持对应版本。1.7.104 使用 SKSE 2.3.1；Media Keys Fix 1.0.2 说明列出 1.7.99，其在 1.7.104 的工作状态仍待确认。

INSTALL / 安装
Exit Skyrim before updating in MO2. Disable SimpleIME and duplicate Text Bridge versions. Personal SkyrimTextBridge.user.ini settings are preserved. The installation ZIP contains only DLL, default INI and this readme.txt with complete license notices; source and symbols are separate.
退出 Skyrim 后通过 MO2 更新，禁用 SimpleIME 和重复的 Text Bridge 版本。保留 SkyrimTextBridge.user.ini 个人配置。安装 ZIP 仅含 DLL、默认 INI 与本 readme.txt（合并完整许可）；源码和符号另包。

LANGUAGE AND FONTS / 语言与字体
UI language automatically follows the Windows display language, not the selected IME. English, Simplified Chinese, Japanese and Korean are available; other languages fall back to English. Missing Language in an older personal INI also means automatic. Optional setting: [Interface] Language=0 automatic, 1 English, 2 Simplified Chinese, 3 Japanese, 4 Korean. F1 -> Skyrim Text Bridge -> Settings changes it too. Page contents update immediately; restart Skyrim to update the sidebar label.
界面自动跟随 Windows 显示语言，不跟随所选输入法；提供英文、简中、日语和韩语，其他语言回退英文。旧个人 INI 缺少 Language 时也默认自动。可选配置：[Interface] Language=0 自动、1 英文、2 简中、3 日语、4 韩语；也可在 F1 -> Skyrim Text Bridge -> Settings 设置。页面内容即时更新，侧栏名称需重启游戏更新。

In SKSEMenuFramework.ini [Fonts], enable the needed EnableChinese/EnableJapanese/EnableKorean ranges and choose a PrimaryFont containing all scripts you intend to display. Put that licensed font in Data/SKSE/Plugins/fonts. Enabling a range does not supply missing glyphs. A multilingual CJK font is useful when switching between languages. No fonts or framework configuration changes are bundled. If the selected UI script is unavailable, the settings page temporarily uses English so controls remain readable. The candidate panel still requires the appropriate glyphs. Restart after changing framework font settings.
在 SKSEMenuFramework.ini 的 [Fonts] 启用所需 EnableChinese/EnableJapanese/EnableKorean 范围，PrimaryFont 必须包含准备显示的所有文字，将有相应许可的字体置于 Data/SKSE/Plugins/fonts。启用字形范围并不会补齐字体缺字，切换多种语言时可使用多语言 CJK 字体。此包不提供字体、不修改框架配置。所选界面语言缺字时设置页暂用英文保证可操作；候选面板仍需要相应字形。修改框架字体后重启游戏。

日本語：Windows の表示言語で UI を自動選択します。Menu Framework の EnableJapanese と日本語対応フォントを設定してください。IME の切替キーで英字・日本語入力を切り替えます。実際の IME はゲーム内テストが必要です。
한국어: Windows 표시 언어에 따라 UI가 자동으로 선택됩니다. Menu Framework에서 EnableKorean을 활성화하고 한국어 지원 글꼴을 설정하세요. 입력기 전환 키로 영문/한글을 전환합니다. 실제 입력기는 게임 내 테스트가 필요합니다.

USE / 使用
Focus a compatible field and press F8. Input follows the current keyboard/IME mode: English can be typed directly; use the IME's own switch key or Ctrl+Space for native/English input. F8 closes the mode. Esc cancels live composition first, then exits after text is delivered; exit requests English for gameplay without selecting another keyboard profile. Configure optional auto activation (off by default), hotkey, enabled state, position, scale, colors and opacity in F1 settings.
聚焦兼容文本框后按 F8，输入跟随当前键盘/输入法状态：英文可直接输入，使用输入法自身切换键或 Ctrl+空格切换本地语言/英文。F8 关闭；Esc 先取消组合文字，文字送达后再退出，退出为游戏操作请求英文，不更换键盘配置。F1 设置可调自动开启（默认关闭）、快捷键、模组开关、位置、缩放、颜色和透明度。

COMPATIBILITY / 兼容性
Input support: Meridian UI/Tailor, Modex, SkyUI and compatible Scaleform fields including RaceMenu naming. Auto activation detects eligible Scaleform/Meridian text fields; the console and Modex remain manual F8. New fields need testing. Prisma UI/Outfit Wheeler retain their own native input; Text Bridge yields and does not provide IME input for Prisma interfaces. Existing Modex Esc/close cleanup and downstream hotkey filtering are retained. Some IMEs do not expose IMM candidate lists. Japanese clause conversion, Korean Hanja conversion and third-party IMEs remain pending live validation.
输入支持：Meridian UI/Tailor、Modex、SkyUI 及兼容 Scaleform 字段（包括 RaceMenu 命名）。自动开启检测符合条件的 Scaleform/Meridian 文本框，控制台与 Modex 保留手动 F8，新字段仍需实测。Prisma UI/Outfit Wheeler 使用自身原生输入，本模组避让，不向 Prisma 界面提供输入法接入。保留 Modex Esc/关闭清理与后续热键过滤。部分输入法不提供 IMM 候选列表，日语分节转换、韩语 Hanja 转换与第三方输入法仍需实际验证。

The panel uses the game's ImGui HUD and framework fonts. Missing glyphs may appear as question marks; the destination editor also needs its own Unicode/font support. No extra rendering device, desktop overlay or emoji fallback is created. Ordinary English and Chinese behavior require regression testing with this beta; exclusive fullscreen and other runtime environments still need game validation.
面板使用游戏内 ImGui HUD 与框架字体，缺失字形可能显示为问号，目标输入框也需要自身的 Unicode/字体支持。不创建额外绘图设备、桌面悬浮窗或 Emoji 补绘。此测试版还需回归实测普通英文、中文，独占全屏及其他运行时环境仍需游戏验证。

LICENSE AND SOURCE / 许可与源码
Copyright (C) 2026 BlackMesa79. Original code: GPL-3.0-only, without warranty. Third-party files retain their licenses; complete notices follow. Corresponding source/build instructions: SkyrimTextBridge-0.4.1-beta.1-source.zip. Provide corresponding source when redistributing binaries. SKSE Menu Framework is installed separately. No restriction is added against modifications or reverse engineering for debugging modifications to an LGPL component.
Copyright (C) 2026 BlackMesa79。自身代码 GPL-3.0-only，不提供担保；第三方文件保留许可，完整声明附于下方。对应源码/构建说明：SkyrimTextBridge-0.4.1-beta.1-source.zip。转发二进制须提供对应源码，SKSE Menu Framework 另行安装；不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
