# Skyrim Text Bridge - Chinese IME Support

天际中文输入支持

Chinese and English typing, in-game candidates and a clean return to gameplay.

中文与英文输入、游戏内候选面板，以及顺畅的输入退出体验。

## Type naturally. Stay in Skyrim. / 在天际，自然输入

Type Chinese directly into supported Skyrim text fields. Skyrim Text Bridge provides an **F8** input mode, an in-game candidate panel, English typing and keyboard hotkey filtering. Optional automatic activation and configurable settings make everyday input easier.

在受支持的天际文本框中直接输入中文。Skyrim Text Bridge 提供 **F8** 输入模式、游戏内候选面板、英文输入与键盘热键过滤，并通过可选自动开启及自定义设置，让日常输入更方便。

The panel and settings currently use Chinese; this description is bilingual. Japanese and Korean localization may be considered later. Dedicated Japanese/Korean IME compatibility has not been validated.

面板及设置界面目前使用中文，本详情页采用英中对照。未来可能考虑日语、韩语本地化，目前尚未专门验证日语或韩语输入法兼容性。

## What's new in 0.4.0 / 0.4.0 更新

**Candidates inside the game.** The panel now uses SKSE Menu Framework's ImGui HUD instead of a desktop overlay. Larger text and tighter spacing improve readability, while position, scale, colors and opacity remain adjustable. The panel does not open the settings menu or pause gameplay.

**候选面板进入游戏画面。** 现在通过 SKSE Menu Framework 的 ImGui HUD 绘制，更大的文字与更紧凑的行距提升可读性，位置、缩放、颜色和透明度仍可调整。面板不会打开设置菜单，也不会暂停游戏。

**Automatic activation by UI framework.** Optional focus detection covers eligible Scaleform fields, including SkyUI searches and RaceMenu character naming, and editable Meridian fields such as Tailor. It detects actual editable focus instead of maintaining a per-mod menu whitelist. **The console remains manual:** opening it does not start Text Bridge; press F8 when needed.

**按 UI 框架自动开启。** 可选焦点检测覆盖符合条件的 Scaleform 文本框，包括 SkyUI 搜索和 RaceMenu 角色命名，以及 Tailor 等 Meridian 可编辑文本框。检测真实可编辑焦点，无需逐个模组建立菜单白名单。**控制台保持手动：**打开控制台不会自动开启，需要时按 F8。

**English typing and cleaner exits.** On entry, Text Bridge follows the current keyboard/IME Chinese or English state. In English state, type directly without first pressing Ctrl+Space. Esc cancels composition, then exits once committed text has been delivered and requests English input for gameplay. Modex Esc/close cleanup prevents accidental menu closure and retained input locks.

**英文直接输入，退出更顺畅。** 进入时跟随当前键盘/输入法的中英文状态，英文状态下直接打字，无需先按 Ctrl+空格。Esc 先取消拼音组合，再于文字送达后退出，并为游戏操作请求英文状态；完善 Modex 的 Esc/关闭清理，避免误关菜单和输入锁残留。

**Runtime update.** CommonLibSSE-NG 10.1.0 adds the retained 1.7.x address/queue support. Exact accepted runtimes are **1.5.97, 1.6.1170, 1.7.99 and 1.7.104**. The validation scope is detailed below.

**运行时更新。** CommonLibSSE-NG 10.1.0 提供现有 1.7.x 地址与字符队列支持，精确接受 **1.5.97、1.6.1170、1.7.99 和 1.7.104**，验证范围见下方说明。

## Input support & compatibility / 输入支持与兼容性

**Input support — Meridian UI (including Tailor), Modex, SkyUI and compatible Scaleform fields.** Text Bridge supplies input in these interfaces. SkyUI and RaceMenu naming use the game's character channel; Meridian uses framework field detection and Unicode submission. Detecting a new field does not guarantee that every custom editor accepts text correctly.

**输入支持——Meridian UI（包括 Tailor）、Modex、SkyUI 及兼容 Scaleform 文本框。** 本模组为这些界面提供输入功能。SkyUI、RaceMenu 命名通过游戏字符通道输入；Meridian 使用框架文本框检测与 Unicode 提交。识别到新字段不等于所有自定义编辑器都能正确接收文字。

**Automatic activation is optional and off by default.** Readonly, disabled, hidden and password fields are excluded; upper input owners block stale fields underneath. Opening the console also blocks underlying automatic targets. Modex and other independent ImGui interfaces remain **manual F8**, since they do not provide a shared reliable text-focus interface.

**自动开启为可选设置，默认关闭。** 排除只读、禁用、隐藏和密码字段，上层输入界面阻止识别底下残留焦点；控制台打开时也不会误触发底下的自动输入目标。Modex 及其他独立 ImGui 界面没有共享的可靠文本焦点接口，继续使用 **F8 手动开启**。

**Coexistence compatibility — Prisma UI and Outfit Wheeler.** Prisma handles its own native input. Text Bridge yields and does not provide or take over IME support for Prisma-based interfaces. It does not repair Prisma's own candidate-window behavior. **Prisma UI is not a Text Bridge dependency.**

**兼容共存——Prisma UI 与 Outfit Wheeler。** Prisma 自行处理原生输入，Text Bridge 主动避让，不为基于 Prisma 的界面提供或接管输入法功能，也不修复 Prisma 自身的候选窗表现。**使用 Text Bridge 不需要安装 Prisma UI。**

**Disable SimpleIME.** Do not enable SimpleIME and Text Bridge together. Install Tailor, Modex and other UI mods with their own required frameworks. Framework versions must independently support your game runtime.

**请禁用 SimpleIME。** 不要同时启用 SimpleIME 与 Text Bridge。Tailor、Modex 和其他界面模组需要安装各自的前置框架，各框架版本也必须独立支持你的游戏运行时。

## Your input. Your settings. / 你的输入，你的设置

**Candidate panel & input method.** Shows composition, IME-provided candidates and the active Windows input method's name. If Windows exposes only language information, it is labeled as keyboard language. The title follows the keyboard/IME's actual Chinese or English state. Emoji display is not supported; missing font glyphs may appear as question marks.

**候选面板与输入法识别。** 显示拼音组合、输入法提供的候选词和当前 Windows 输入法名称；Windows 只提供语言信息时，标注为键盘语言。标题跟随当前键盘/输入法实际中英文状态。不支持 Emoji 显示，字体缺失的字形可能显示为问号。

**Hotkey filtering.** Ordinary keyboard presses, held events and releases are filtered from the downstream game input chain while Text Bridge is active. Mouse and common editing keys remain available; Ctrl+A/C/X/V pass through outside composition when supported by the target editor. Editing-key hotkeys, earlier hooks and independent keyboard polling may still react. Prisma native input is not filtered.

**热键过滤。** 本模组输入期间，过滤发往后续游戏输入链的普通键按下、持续和松开事件，保留鼠标与常用编辑键。非选词期间允许 Ctrl+A/C/X/V 通过，实际操作取决于目标编辑器。编辑键热键、较早钩子和独立键盘轮询仍可能响应，Prisma 原生输入不应用本模组过滤。

**In-game customization.** Open **SKSE Menu Framework (F1 by default) → Skyrim Text Bridge → 输入法与面板设置** to adjust enabled state, hotkey, automatic activation, position, scale, colors and background opacity. Text opacity remains independent of the background. Save to retain settings between sessions.

**游戏内自定义。** 打开 **SKSE Menu Framework（默认 F1）→ Skyrim Text Bridge → 输入法与面板设置**，调整模组开关、快捷键、自动开启、位置、缩放、颜色及背景不透明度。文字透明度独立于背景，点击保存后下次启动继续使用。

## Before you start / 前置要求

**Required: Windows, a Chinese IME, SKSE64 and Address Library matching your runtime, [Media Keys Fix SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/92948), and SKSE Menu Framework with HUD/ImGui exports.** Menu Framework is now required for the candidate panel as well as settings. Configure **EnableChinese=true** and a valid Chinese **PrimaryFont** in SKSEMenuFramework.ini; place the font under Data/SKSE/Plugins/fonts. Local public API checks used Menu Framework 3.18.

**必需：Windows、中文输入法、匹配运行时的 SKSE64 与 Address Library、[Media Keys Fix SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/92948)，以及提供 HUD/ImGui 导出的 SKSE Menu Framework。** Menu Framework 现在同时用于候选面板和设置页，属于必需前置。请在 SKSEMenuFramework.ini 中设置 **EnableChinese=true** 与有效中文 **PrimaryFont**，字体放在 Data/SKSE/Plugins/fonts 下。本地公开接口检查使用 Menu Framework 3.18。

**Media Keys Fix SKSE supplies the nonexclusive keyboard environment needed by Windows IME messages.** Without it, the panel may open while text/composition/candidates fail. An equivalent keyboard patch may work but must be verified separately. This dependency does not require Prisma UI.

**Media Keys Fix SKSE 提供 Windows 输入法消息所需的非独占键盘环境。** 缺少它时，可能出现面板能打开，但文字、拼音组合或候选词不工作的情况。等效键盘补丁可以自行验证，本依赖不意味着需要安装 Prisma UI。

**Supported runtime admission: 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104 only.** All four passed offline address-library, character-queue and layout checks. The user approved current behavior after testing on **1.6.1170**. **1.5.97, 1.7.99 and 1.7.104 still await full in-game validation.** Other patches, GOG 1.6.1179 and Skyrim VR are not admitted.

**运行时准入仅限 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104。** 四版本均通过离线地址库、字符队列及布局检查，用户已在 **1.6.1170** 游戏测试后确认当前功能并批准发布。**1.5.97、1.7.99 和 1.7.104 仍待完整游戏验证。** 未放行其他补丁、GOG 1.6.1179 或 Skyrim VR。

For **1.7.104**, use SKSE 2.3.1 and the matching Address Library. Media Keys Fix 1.0.2 documents 1.7.99; its 1.7.104 behavior has not been confirmed here. Install compatible versions of every prerequisite; updating Text Bridge alone does not establish their compatibility.

**1.7.104** 使用 SKSE 2.3.1 与对应 Address Library。Media Keys Fix 1.0.2 说明列出 1.7.99，本项目尚未确认其在 1.7.104 的表现。请为每项前置安装兼容版本，仅更新 Text Bridge 不代表其他依赖也已兼容。

## Get started / 安装与使用

Install the release ZIP with MO2 or your mod manager. It contains only the DLL, default INI and a **readme.txt** with usage and complete license notices. Exit Skyrim before updating, enable only one Text Bridge version and disable SimpleIME. Preserve **SkyrimTextBridge.user.ini**; source and debugging symbols are separate downloads.

通过 MO2 或其他模组管理器安装发布 ZIP，包内只有 DLL、默认 INI 和包含使用说明及完整许可的 **readme.txt**。更新前退出游戏，只启用一个 Text Bridge 版本并禁用 SimpleIME；保留 **SkyrimTextBridge.user.ini**，源码和调试符号另包提供。

Click a supported field and press **F8**. In English state, type normally; for Chinese, use the IME's own switch key or optional **Ctrl+Space**. Changing mode cancels uncommitted composition, so select the candidate first. F8 also closes input. A configured custom hotkey replaces F8 throughout these instructions.

点击支持的文本框，按 **F8**。英文状态下正常打字，需要中文时使用输入法自身切换键或可选 **Ctrl+空格**。切换会取消未提交的拼音，请先选词；F8 也可关闭输入。如自定义了快捷键，本文所有 F8 操作改用你的快捷键。

Press **Esc** after finishing input. Active composition is cancelled first; queued committed characters are delivered before exit, so press Esc again if needed. The exit key press/release does not trigger the UI's normal Esc action; a later press can close it. Exit requests English state while retaining the selected Windows IME profile. Release any movement key held during typing before pressing it again.

输入完成后按 **Esc**，有拼音时先取消，已选文字仍在队列中时先等待送达，必要时再按一次退出。退出的按下/松开不触发界面原有 Esc 操作，之后再按可正常关闭。退出请求英文状态，保留所选 Windows 输入法配置；输入期间仍按住的移动键需先松开再重新按下。

For automatic activation, enable **文本框聚焦时自动开启** in the settings page and save, or set **AutoEnable=1** under **[Input]** in the user INI. Manual F8/Esc exit suppresses reopening for the current focus; leave and refocus to rearm. Automatic sessions end on blur; changing menus, loading saves, opening settings or losing game focus also ends input.

自动开启需在设置页勾选 **文本框聚焦时自动开启** 并保存，或在个人 INI 的 **[Input]** 下设置 **AutoEnable=1**。手动 F8/Esc 退出后当前焦点不会立即重开，离开再聚焦后恢复；自动会话失焦后结束，切换菜单、读档、打开设置或游戏失焦也会退出输入。

## Things to know / 使用须知

Some IMEs do not expose candidates through IMM, so this panel cannot show their lists. Input-method names depend on Windows information. Emoji rendering is intentionally unsupported; the target editor's own fonts and Unicode handling also determine text display. Alternative layouts, dead keys and newly detected editors may need additional testing.

部分输入法不通过 IMM 提供候选列表，本面板无法展示此类候选词；名称识别取决于 Windows 信息。目前明确不支持 Emoji 渲染，目标编辑器的显示还取决于其自身字体与 Unicode 处理。其他键盘布局、死键和新识别编辑器可能需要额外测试。

The panel is drawn inside the game, removing the desktop-overlay restriction. Windowed and borderless use are covered by current local testing; true exclusive fullscreen, Alt+Tab and rendering-mod combinations still need game validation. It does not install an IME, translate text or modify a context already owned by Prisma or another framework.

面板已绘制在游戏内，取消桌面悬浮窗的模式限制。当前本地测试覆盖窗口化/无边框使用，真正独占全屏、Alt+Tab 与渲染模组组合仍需游戏验证。本模组不安装输入法、不翻译文字，也不修改已由 Prisma 或其他框架接管的输入上下文。

## Open source & feedback / 开源与反馈

Original Text Bridge code is licensed under **GPL-3.0-only**. Third-party dependencies retain their own licenses; full notices are included in readme.txt. [GitHub source](https://github.com/BlackMesa79/Skyrim-Text-Bridge) and a matching source ZIP are provided. For issues, include runtime, IME, related mod versions, reproduction steps and SkyrimTextBridge.log. Entered text is not logged.

自身代码采用 **GPL-3.0-only**，第三方依赖保留原有许可，完整声明见 readme.txt。提供 [GitHub 源码](https://github.com/BlackMesa79/Skyrim-Text-Bridge) 与对应源码 ZIP。反馈请附游戏运行时、输入法、相关模组版本、复现步骤及 SkyrimTextBridge.log，日志不记录输入文字。
