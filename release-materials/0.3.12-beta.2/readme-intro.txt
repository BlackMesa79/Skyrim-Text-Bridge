SKYRIM TEXT BRIDGE 0.3.12-beta.2
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

STATUS / 状态
Local test build for an in-game ImGui candidate/status panel, retaining RaceMenu automatic activation from beta.1. Stable 0.3.11 remains available. Runtime whitelist: 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. This new render path, RaceMenu naming and true exclusive fullscreen still need game testing; the older build's 1.6.1170 feedback does not validate this binary. 1.5.97 and both 1.7 versions also remain unverified in game.
本地测试版，将候选/状态面板改为游戏内 ImGui 绘制，并保留 beta.1 的 RaceMenu 自动开启修复。保留 0.3.11 正式包，运行时白名单仍为 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104。新绘制方案、RaceMenu 命名及真正独占全屏仍待游戏实测，旧版 1.6.1170 反馈不代表新 DLL 已实测；1.5.97 和两个 1.7 版本也仍待游戏验证。

CHANGES / 更新
Replaces the separate Windows layered window with ImGui foreground draw commands through SKSE Menu Framework's per-frame HUD callback. Works without opening the F1 settings menu, does not create an interactive ImGui window and does not request mouse/keyboard focus or pause the game. Keeps position, scale, colors, background opacity, selected candidate highlight, IME name and timed exit notifications. Thread-safe snapshots separate IME/window callbacks from rendering; render callbacks do not access IMM contexts. No extra Direct3D hook, context or font atlas is created by Text Bridge.
通过 SKSE Menu Framework 每帧 HUD 回调和 ImGui 前景绘制命令，替换独立 Windows 分层窗口。不需要打开 F1 设置菜单，不创建可交互 ImGui 窗口，不申请鼠标/键盘焦点，也不暂停游戏。保留位置、缩放、配色、背景透明度、候选高亮、输入法名称及定时关闭提示。用线程安全快照隔离输入法/窗口回调与渲染，渲染回调不访问 IMM 上下文。Text Bridge 不额外创建 Direct3D 钩子、ImGui 上下文或字体图集。

DISPLAY MODES / 显示模式
The panel is now drawn into the game's render output rather than a desktop overlay. Windowed, borderless and true exclusive fullscreen are intended targets, but exclusive fullscreen, resolution transitions, Alt+Tab and render-mod combinations need in-game confirmation. Text delivery still requires a nonexclusive keyboard patch. Native IMEs that do not expose IMM candidate lists are not made supported merely by changing the renderer. Prisma retains its own candidate rendering.
面板现已绘制到游戏渲染输出中。目标显示模式包含窗口化、无边框和真正独占全屏，但独占全屏、分辨率变化、Alt+Tab 以及渲染模组组合仍需游戏验证。文字输入仍依赖非独占键盘补丁；输入法不提供 IMM 候选列表的问题不会因更换渲染器自动解决。Prisma 仍使用自己的候选绘制。

REQUIREMENTS / 前置
SKSE Menu Framework is now REQUIRED for the panel and its HUD/ImGui API. Use a build compatible with the game runtime; local export audit passed on 3.18. Missing required exports leave the Text Bridge input hook disabled and report the cause in SkyrimTextBridge.log. Framework settings UI remains available when its own exports work. No framework DLL is bundled.
SKSE Menu Framework 现为面板及其 HUD/ImGui API 的必需前置。使用匹配游戏运行时的版本，本地已通过 3.18 导出接口检查。缺少所需接口时不安装 Text Bridge 输入钩子，SkyrimTextBridge.log 会记录原因；框架设置页接口可用时仍能打开。框架 DLL 不随包提供。

Chinese font setup: in SKSEMenuFramework.ini under [Fonts], set EnableChinese=true and PrimaryFont to the name of an installed Chinese-capable font in Data/SKSE/Plugins/fonts (for example MiSans-Regular.ttf). Install the font separately under its own license and restart Skyrim. Existing framework Chinese font setups can be reused. Text Bridge logs a warning if the active font lacks Chinese glyphs; this package does not redistribute Microsoft or MiSans fonts or modify another mod's font configuration.
中文字体：在 SKSEMenuFramework.ini 的 [Fonts] 下设置 EnableChinese=true，PrimaryFont 指向 Data/SKSE/Plugins/fonts 中已安装的中文字体文件名（如 MiSans-Regular.ttf）。按字体本身许可另行安装并重启游戏；已配置框架中文字体的用户可以沿用。字体缺少中文字形时 Text Bridge 会记录提示，本包不分发微软/MiSans 字体，也不会修改其他模组字体配置。

Also requires Windows, matching SKSE64 and Address Library, and Media Keys Fix SKSE or an equivalent working nonexclusive keyboard patch. Skyrim 1.7.104 uses SKSE 2.3.1. Media Keys Fix 1.0.2 advertises 1.7.99; 1.7.104 operation is still unverified. UI framework runtime compatibility must be checked independently.
仍需 Windows、匹配的 SKSE64 和 Address Library，以及可用的 Media Keys Fix SKSE 或等效非独占键盘补丁。1.7.104 对应 SKSE 2.3.1，Media Keys Fix 1.0.2 的说明列出 1.7.99，其在 1.7.104 工作状态仍待验证。各 UI 框架的运行时兼容性需独立核对。

INSTALL AND ROLLBACK / 安装与回退
Exit Skyrim before updating through MO2. Disable SimpleIME and any duplicate Text Bridge installation (including earlier test packages). Test your intended display mode. Keep SkyrimTextBridge.user.ini to preserve personal settings. Rollback to the previous 0.3.11 package is possible without save edits; back up your settings first.
退出游戏后通过 MO2 更新。禁用 SimpleIME 及重复的 Text Bridge 安装（包括旧测试版），请测试目标显示模式。保留 SkyrimTextBridge.user.ini 可沿用个人设置。如需回退，可重新安装之前的 0.3.11 包，无需修改存档；请先备份个人设置。

INPUT SUPPORT / 输入支持
Meridian UI (including Tailor), Modex and SkyUI: focus a supported field and press F8 to use Text Bridge. Ctrl + Space switches Chinese/English. F8 exits. Esc cancels ongoing composition first; after completed input it exits and closes the restored game IME context so movement keys do not become pinyin. If committed characters are still queued, wait for delivery and press Esc again.
Meridian UI（如 Tailor）、Modex、SkyUI：聚焦受支持文本框，按 F8 调用 Text Bridge。Ctrl + 空格切换中英文，F8 关闭。Esc 先取消选词中的拼音；输入完成后退出并关闭恢复后的游戏 IME 上下文，避免移动键变成拼音。如果文字尚在提交队列中，请等送达后再按一次 Esc。

PRISMA COMPATIBILITY / Prisma 兼容性
Prisma UI and Outfit Wheeler use their own native input. Text Bridge yields to Prisma and does not provide IME support for Prisma-based interfaces. The runtime-update build was user-tested on 1.6.1170; 1.7.x game testing is pending.
Prisma UI 与 Outfit Wheeler 使用自身原生输入。Text Bridge 主动避让，不为 Prisma 界面提供输入法支持。运行时更新版已由用户在 1.6.1170 回归测试；1.7.x 游戏验证仍待完成。

SETTINGS / 设置
SKSE Menu Framework settings: F1 -> Skyrim Text Bridge -> 输入法与面板设置. Customize enabled state, hotkey, panel position, size, colors and opacity. Save to persist settings. Optional automatic activation is off by default; it covers SkyUI item-menu searches, RaceMenu naming and other focused editable fields, and Meridian/Tailor editable fields. Modex stays manual; Prisma retains native input. Manual off suppresses reopening until focus leaves and returns.
SKSE Menu Framework 设置：F1 -> Skyrim Text Bridge -> 输入法与面板设置。可调整开关、热键、面板位置、大小、颜色及透明度，保存后保留。自动开启默认关闭，覆盖 SkyUI 物品菜单搜索框、RaceMenu 命名等聚焦可编辑文本框，以及 Meridian/Tailor 文本框；Modex 手动开启，Prisma 使用原生输入。手动关闭后，离开文本框再重新聚焦才会自动重开。

TESTING / 测试
First check loading, settings registration, Chinese candidates, English typing, Esc exit/movement, auto activation, and Prisma coexistence on the existing 1.6.1170 setup. Also test RaceMenu naming, cancel/reopen, slider editing, preset dialogs, and Esc exit. Then repeat on 1.7.99 and 1.7.104 with matching prerequisites. Offline checks include 61 core, 43 direct IME, and 78 runtime queue/address assertions plus 30 ImGui panel and 15 Scaleform focus assertions and DOM/API/settings/metadata checks. These do not prove executable hook correctness or end-to-end game input. For a failure, provide the game/SKSE/prerequisite versions and SkyrimTextBridge.log plus skse64.log; avoid including private typed content.
先在现有 1.6.1170 环境回归检查加载、设置页注册、中文候选、英文输入、Esc 退出及移动、自动开启和 Prisma 共存，增加检查 RaceMenu 命名、取消并重新开启、滑块操作、预设对话框及 Esc 退出，再在前置匹配的 1.7.99 与 1.7.104 上重复。离线检查包括 61 项核心、43 项直接 IME、78 项运行时队列/地址断言以及 30 项 ImGui 面板与 15 项 Scaleform 焦点断言和 DOM/API/设置/元数据检查，不能证明游戏钩子位置及完整输入流程。出现问题时提供游戏、SKSE 和前置版本，以及 SkyrimTextBridge.log 与 skse64.log；避免附带私人输入内容。

LIMITATIONS / 限制
Keyboard filtering covers downstream ordinary events; editing keys, earlier hooks and independent polling can still activate shortcuts. Some IMEs do not expose IMM candidate lists. Chinese UI only; Japanese/Korean localization remains a possible future direction. Retains the optional native input-state/reset API; no client is required to use it.
热键过滤覆盖后续普通键盘事件，编辑键、较早钩子与独立轮询仍可能触发快捷键。部分输入法不提供 IMM 候选列表。目前界面为中文，日语/韩语本地化仍为未来可能方向。保留可选原生输入状态/重置接口，不要求其他模组接入。

LICENSE AND SOURCE / 许可与源码
Copyright (C) 2026 BlackMesa79. Original code: GPL-3.0-only, without warranty. CommonLibSSE-NG 10.1.0: GPL-3.0-or-later with its additional permissions; original third-party notices remain intact. Full licenses and notices follow in this same file. Corresponding source and build instructions accompany this release in SkyrimTextBridge-0.3.12-beta.2-source.zip. Do not distribute the binary without also making its corresponding source available.
Copyright (C) 2026 BlackMesa79。本项目自身代码采用 GPL-3.0-only，不提供担保；CommonLibSSE-NG 10.1.0 采用 GPL-3.0-or-later 及其附加许可，保留第三方原有声明。全文合并于本文件下方。对应源码及构建说明随本版本提供于 SkyrimTextBridge-0.3.12-beta.2-source.zip；转发二进制时也应提供对应源码。

Required SKSE Menu Framework is separately installed and not bundled. Its API header is included in the source tree. No restriction is added against modification or reverse engineering for debugging modifications to an LGPL-covered component.
SKSE Menu Framework 另行安装，不随包提供；源码内包含 API 头文件。不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
