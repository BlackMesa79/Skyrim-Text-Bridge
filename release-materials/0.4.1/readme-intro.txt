SKYRIM TEXT BRIDGE 0.4.1
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

Chinese Japanese and Korean IME Support / 中文、日语与韩语输入法支持

Type naturally. Stay in Skyrim. / 在天际，自然输入
Type into supported Skyrim fields with F8, an in-game candidate panel, direct English typing and keyboard hotkey filtering. Optional automatic activation starts input when a supported text field gains focus.
使用 F8 在受支持的天际文本框内输入，提供游戏内候选面板、直接英文输入与键盘热键过滤。可选自动开启可在支持的文本框聚焦时启动输入。

The interface now supports English, Simplified Chinese, Japanese and Korean, selected automatically from the Windows display language. Other languages fall back to English. The selected IME does not change the UI language; a manual override is optional.
界面现支持英文、简体中文、日语和韩语，默认根据 Windows 显示语言自动识别，其他语言回退英文。所选输入法不会改变界面语言，也可手动覆盖。

Input support & compatibility / 输入支持与兼容性
Input support: Meridian UI (including Tailor), Modex, SkyUI and compatible Scaleform fields, including RaceMenu character naming. Text Bridge supplies input to these interfaces. Detecting a new field does not guarantee that every custom editor accepts text correctly.
输入支持：Meridian UI（包括 Tailor）、Modex、SkyUI 及兼容 Scaleform 文本框，包括 RaceMenu 角色命名。 本模组为这些界面提供输入功能。检测到新文本框不代表所有自定义编辑器都能正确接收文字。

Optional automatic activation covers eligible Scaleform and Meridian fields and is off by default. Password, hidden, disabled and read-only fields are excluded. The console and Modex remain manual F8; opening the console also blocks automatic activation of fields underneath.
可选自动开启覆盖符合条件的 Scaleform 与 Meridian 字段，默认关闭，排除密码、隐藏、禁用和只读字段。控制台与 Modex 保持手动 F8，控制台打开时也阻止底下残留焦点自动开启。

Coexistence: Prisma UI / Outfit Wheeler. Prisma uses its own native input. Text Bridge yields to it and does not provide or take over IME support for Prisma interfaces, or fix Prisma candidate-window behavior. Prisma is not a Text Bridge dependency.
兼容共存：Prisma UI / Outfit Wheeler。 Prisma 使用自身原生输入，Text Bridge 主动避让，不为 Prisma 界面提供或接管输入法支持，也不修复 Prisma 自身候选窗表现。使用本模组不需要安装 Prisma。

Disable SimpleIME and duplicate Text Bridge installations. Install each UI mod with its own required framework. Retains Modex Esc/close cleanup. Independent ImGui interfaces other than those tested are not automatically considered supported.
请禁用 SimpleIME 和重复的 Text Bridge 版本，各界面模组仍需要自己的前置框架。保留 Modex Esc/关闭清理，其他未测试的独立 ImGui 界面不自动视为已支持。

Before you start / 前置要求
Required: Windows and an installed IME, matching SKSE64 and Address Library, Media Keys Fix SKSE (https://www.nexusmods.com/skyrimspecialedition/mods/92948), and SKSE Menu Framework with the required HUD/ImGui exports. The framework is required for both the candidate panel and settings; local interface checks use version 3.18.
必需：Windows 与已安装的输入法、匹配的 SKSE64 和 Address Library、Media Keys Fix SKSE (https://www.nexusmods.com/skyrimspecialedition/mods/92948)，以及有相应 HUD/ImGui 导出的 SKSE Menu Framework。 框架同时用于候选面板和设置页，本地接口检查使用 3.18。

Media Keys Fix SKSE supplies the nonexclusive keyboard environment needed for Windows IME messages. Without it, the panel may open while text or candidates fail. Equivalent keyboard patches require separate verification; this prerequisite does not require Prisma UI.
Media Keys Fix SKSE 提供 Windows 输入法消息所需的非独占键盘环境，缺少时可能面板能打开但文字或候选不工作。等效键盘补丁需自行验证，此前置不意味着需要安装 Prisma UI。

Fonts are required for the languages you display. In SKSEMenuFramework.ini, enable the needed EnableChinese / EnableJapanese / EnableKorean options and choose a PrimaryFont containing those glyphs under Data/SKSE/Plugins/fonts. Enabling a range alone cannot supply missing glyphs. Use a multilingual CJK font for multiple languages and restart after changes. Fonts are not bundled; Text Bridge does not modify the shared framework configuration.
字体必须支持准备显示的语言。 在 SKSEMenuFramework.ini 启用需要的 EnableChinese / EnableJapanese / EnableKorean，并设置 Data/SKSE/Plugins/fonts 下包含相应字形的 PrimaryFont，仅启用范围无法补齐缺字。多语言可使用多语言 CJK 字体，修改后重启。字体不随包提供，本模组不修改共享框架配置。

Exact admitted runtimes: 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. All four pass offline address/queue/layout checks. Previous stable input was user-tested on 1.6.1170; the new multilingual behavior and other runtimes need additional game testing. Other patches, GOG 1.6.1179 and VR are not admitted.
精确准入版本：1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104。 四版本通过离线地址/队列/布局检查，之前稳定输入已在 1.6.1170 获用户实测；新增多语言功能与其他运行时需要更多游戏测试。未放行其他补丁、GOG 1.6.1179 或 VR。

Every prerequisite must independently support your game version. 1.7.104 uses SKSE 2.3.1 and its matching Address Library; Media Keys Fix 1.0.2 documents 1.7.99, and its 1.7.104 operation remains unverified here.
各前置必须独立支持你的游戏版本。1.7.104 使用 SKSE 2.3.1 与对应 Address Library；Media Keys Fix 1.0.2 说明列出 1.7.99，本项目尚未确认其在 1.7.104 的工作状态。

Get started / 安装与使用
Exit Skyrim, install the ZIP through MO2 or your mod manager, and enable one Text Bridge version. The package contains only DLL and readme.txt, including complete license notices. Preserve SkyrimTextBridge.user.ini when replacing an installation; back it up before removing/reinstalling a mod. MO2-generated settings may be in Overwrite or a configured output mod.
退出 Skyrim 后通过 MO2 或其他管理器安装 ZIP，只启用一个 Text Bridge 版本。安装包只有 DLL 和 readme.txt，后者合并完整许可。替换安装时保留 SkyrimTextBridge.user.ini，删除或重装模组前先备份；MO2 新生成的配置可能位于 Overwrite 或指定输出模组。

Click a supported field and press F8. English state accepts normal English typing; use your IME's own language key or optional Ctrl+Space for native input. The latter cancels uncommitted composition. F8 closes the mode. A custom hotkey replaces F8 throughout these instructions.
点击支持的文本框后按 F8，英文状态可正常输入英文，使用输入法自身切换键或可选 Ctrl+空格 切换本地语言输入；后者会取消未提交的组合文字。F8 也可关闭，自定义快捷键后用你的按键替代本文 F8。

Esc first cancels active composition, then exits after committed text is delivered; press again if needed. The exit press/release is consumed so it does not close Modex. Exit requests English state for gameplay without selecting another Windows input profile.
Esc 先取消组合文字，再于已选文字送达后退出，必要时再按一次。退出的按下/松开被拦截，避免透传关闭 Modex；退出请求英文状态以便游戏操作，不更换 Windows 输入法配置。

Open F1 → Skyrim Text Bridge → Input & panel (localized automatically) to configure language, enabled state, hotkey, automatic activation, position, scale, colors and opacity. Click Save settings to create/update SkyrimTextBridge.user.ini. Page language updates immediately; the sidebar label updates after restart. Configuration is stored outside Skyrim saves.
打开 F1 → Skyrim Text Bridge → 输入与面板（名称自动本地化），调整语言、开关、快捷键、自动开启、位置、缩放、颜色和透明度。点击保存设置才创建/更新 SkyrimTextBridge.user.ini。页面语言即时更新，侧栏名称重启后更新；配置独立于 Skyrim 存档保存。

Automatic activation is off by default. Enable it and save to use focused fields. Manual F8/Esc exit suppresses immediate reactivation until focus leaves and returns. Automatic sessions end on blur; menu changes, loading saves, opening settings and losing game focus also end input.
自动开启默认关闭，在设置页启用并保存。手动 F8/Esc 退出后，当前焦点不会立即重开，离开再聚焦才恢复；自动会话失焦后结束，切换菜单、读档、打开设置和游戏失焦也会退出输入。

Things to know / 使用须知
The in-game ImGui panel displays composition, IME-provided candidates and the current Windows input method name. Position, size, colors and background opacity are adjustable. Emoji rendering is unsupported; the destination editor needs its own font/Unicode support too. Missing interface glyphs temporarily use English settings help; candidates still require the relevant font.
游戏内 ImGui 面板显示组合文字、输入法提供的候选及当前 Windows 输入法名称，可调位置、大小、颜色和背景透明度。不支持 Emoji 渲染，目标编辑器也需自身的字体/Unicode 支持。界面缺字时设置页暂用英文保证可操作，候选仍需要相应字体。

Some IMEs do not expose IMM candidate lists. Japanese clause conversion, Korean Hanja candidates and third-party IMEs need real-engine testing; simulated composition checks do not establish universal IME support. The mod does not install an IME or translate typed text.
部分输入法不提供 IMM 候选列表。日语分节转换、韩语 Hanja 候选及第三方输入法需真实测试，模拟组合检查不代表支持所有输入法。本模组不安装输入法、不翻译输入文字。

Hotkey filtering applies to ordinary downstream keyboard events while active. Mouse and editing keys remain usable; Ctrl+A/C/X/V pass through outside composition when supported by the editor. Earlier hooks, independent polling and hotkeys bound to editing keys may still react. Prisma input is not filtered.
热键过滤作用于输入期间后续游戏输入链的普通键事件，保留鼠标和编辑键；非组合期间允许 Ctrl+A/C/X/V 通过，取决于编辑器。较早钩子、独立轮询和绑定在编辑键的热键仍可能响应，Prisma 输入不应用此过滤。

The panel is rendered in-game. True exclusive fullscreen, Alt+Tab and rendering-mod combinations still require game validation. No private rendering device, desktop overlay or emoji fallback is created.
面板在游戏内绘制，真正独占全屏、Alt+Tab 和渲染模组组合仍需游戏验证。不创建私有绘图设备、桌面悬浮窗或 Emoji 补绘。

LICENSE AND SOURCE / 许可与源码
Copyright (C) 2026 BlackMesa79. Original code: GPL-3.0-only, without warranty. Third-party files retain their original licenses; complete notices follow. Corresponding source and build instructions: SkyrimTextBridge-0.4.1-source.zip. Provide corresponding source when redistributing binaries. SKSE Menu Framework is installed separately. No restriction is added against modifications or reverse engineering for debugging modifications to an LGPL component.
Copyright (C) 2026 BlackMesa79。自身代码采用 GPL-3.0-only，不提供担保；第三方保留原许可，完整声明附于下方。对应源码与构建说明：SkyrimTextBridge-0.4.1-source.zip，转发二进制须提供对应源码。SKSE Menu Framework 另行安装，不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
