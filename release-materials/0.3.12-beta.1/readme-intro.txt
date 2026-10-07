SKYRIM TEXT BRIDGE 0.3.12-beta.1
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

STATUS / 状态
This is a local test build for automatic activation in RaceMenu character-name fields. Stable 0.3.11 remains available. Exact runtime whitelist remains 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. RaceMenu automatic activation still needs an in-game test; 1.5.97 and both 1.7 runtimes remain unverified in game.
本包用于本地测试 RaceMenu 角色命名框自动开启，保留 0.3.11 正式包。运行时白名单仍为 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104。RaceMenu 自动开启仍需游戏实测；1.5.97 和两个 1.7 版本的游戏验证状态不变。

CHANGES / 更新
Adds RaceSex Menu (used by RaceMenu) to the existing automatic focus scanner. Requires engine text-input permission, an actual focused editable field, and visible/enabled field ancestors. Customization sliders alone do not start input; hidden dialogs with stale focus are rejected. The existing optional AutoEnable setting remains off by default.
将 RaceMenu 使用的 RaceSex Menu 纳入已有自动焦点检测。要求游戏允许文字输入、真正聚焦可编辑文本框，且文本框及父控件可见可用；仅操作捏脸滑块不会开启，关闭对话框后残留的焦点会被排除。沿用原有可选 AutoEnable 设置，默认关闭。

DISPLAY MODE / 显示模式
Use windowed or borderless mode. The current candidate/status panel is a separate Windows layered window, not drawn into the game swap chain; visibility in true exclusive fullscreen is not supported or guaranteed. Windows fullscreen optimizations may make some apparent fullscreen setups behave as borderless, but that is not true-exclusive support. Reliable exclusive-fullscreen candidates require an in-game renderer and separate validation; this build does not add one. Media Keys Fix changes keyboard cooperative access, not display mode.
使用窗口化或无边框模式。目前候选/状态面板是独立 Windows 分层窗口，没有绘制到游戏交换链中，不支持或保证真正独占全屏时可见。Windows 全屏优化可能让部分看似全屏的环境按无边框方式工作，但这不代表支持真正独占全屏。可靠支持需要游戏内绘制方案及独立验证，本包尚未加入。Media Keys Fix 改变的是键盘访问方式，不是显示模式。

REQUIREMENTS / 前置
Windows, matching SKSE64 and Address Library, and Media Keys Fix SKSE or an equivalent nonexclusive keyboard patch that works on your runtime. Skyrim 1.7.104 uses SKSE 2.3.1 and its matching Address Library. The locally installed Media Keys Fix 1.0.2 lists 1.7.99; its 1.7.104 operation has not been verified. Updating Text Bridge does not update other mods. Optional SKSE Menu Framework and UI frameworks also need their own compatible builds.
需要 Windows、匹配游戏的 SKSE64 和 Address Library，以及能在该游戏版本上工作的 Media Keys Fix SKSE 或等效非独占键盘补丁。1.7.104 对应 SKSE 2.3.1 及匹配的地址库。本地 Media Keys Fix 1.0.2 的说明列出 1.7.99，其在 1.7.104 上的工作状态尚未验证。Text Bridge 更新不会替其他模组完成适配；可选的 SKSE Menu Framework 和 UI 框架也需各自匹配版本。

INSTALL AND ROLLBACK / 安装与回退
Exit Skyrim before updating through MO2. Disable SimpleIME and any duplicate Text Bridge installation (including earlier test packages). Use windowed or borderless mode. Keep SkyrimTextBridge.user.ini to preserve personal settings. Rollback to the previous 0.3.11 package is possible without save edits; back up your settings first.
退出游戏后通过 MO2 更新。禁用 SimpleIME 及重复的 Text Bridge 安装（包括旧测试版），使用窗口化或无边框模式。保留 SkyrimTextBridge.user.ini 可沿用个人设置。如需回退，可重新安装之前的 0.3.11 包，无需修改存档；请先备份个人设置。

INPUT SUPPORT / 输入支持
Meridian UI (including Tailor), Modex and SkyUI: focus a supported field and press F8 to use Text Bridge. Ctrl + Space switches Chinese/English. F8 exits. Esc cancels ongoing composition first; after completed input it exits and closes the restored game IME context so movement keys do not become pinyin. If committed characters are still queued, wait for delivery and press Esc again.
Meridian UI（如 Tailor）、Modex、SkyUI：聚焦受支持文本框，按 F8 调用 Text Bridge。Ctrl + 空格切换中英文，F8 关闭。Esc 先取消选词中的拼音；输入完成后退出并关闭恢复后的游戏 IME 上下文，避免移动键变成拼音。如果文字尚在提交队列中，请等送达后再按一次 Esc。

PRISMA COMPATIBILITY / Prisma 兼容性
Prisma UI and Outfit Wheeler use their own native input. Text Bridge yields to Prisma and does not provide IME support for Prisma-based interfaces. The runtime-update build was user-tested on 1.6.1170; 1.7.x game testing is pending.
Prisma UI 与 Outfit Wheeler 使用自身原生输入。Text Bridge 主动避让，不为 Prisma 界面提供输入法支持。运行时更新版已由用户在 1.6.1170 回归测试；1.7.x 游戏验证仍待完成。

SETTINGS / 设置
Optional SKSE Menu Framework: F1 -> Skyrim Text Bridge -> 输入法与面板设置. Customize enabled state, hotkey, panel position, size, colors and opacity. Save to persist settings. Optional automatic activation is off by default; it covers SkyUI item-menu searches, RaceMenu naming and other focused editable fields, and Meridian/Tailor editable fields. Modex stays manual; Prisma retains native input. Manual off suppresses reopening until focus leaves and returns.
可选 SKSE Menu Framework：F1 -> Skyrim Text Bridge -> 输入法与面板设置。可调整开关、热键、面板位置、大小、颜色及透明度，保存后保留。自动开启默认关闭，覆盖 SkyUI 物品菜单搜索框、RaceMenu 命名等聚焦可编辑文本框，以及 Meridian/Tailor 文本框；Modex 手动开启，Prisma 使用原生输入。手动关闭后，离开文本框再重新聚焦才会自动重开。

TESTING / 测试
First check loading, settings registration, Chinese candidates, English typing, Esc exit/movement, auto activation, and Prisma coexistence on the existing 1.6.1170 setup. Also test RaceMenu naming, cancel/reopen, slider editing, preset dialogs, and Esc exit. Then repeat on 1.7.99 and 1.7.104 with matching prerequisites. Offline checks include 61 core, 43 direct IME, and 78 runtime queue/address assertions plus 15 Scaleform focus assertions and DOM/API/settings/metadata checks. These do not prove executable hook correctness or end-to-end game input. For a failure, provide the game/SKSE/prerequisite versions and SkyrimTextBridge.log plus skse64.log; avoid including private typed content.
先在现有 1.6.1170 环境回归检查加载、设置页注册、中文候选、英文输入、Esc 退出及移动、自动开启和 Prisma 共存，增加检查 RaceMenu 命名、取消并重新开启、滑块操作、预设对话框及 Esc 退出，再在前置匹配的 1.7.99 与 1.7.104 上重复。离线检查包括 61 项核心、43 项直接 IME、78 项运行时队列/地址断言以及 15 项 Scaleform 焦点断言和 DOM/API/设置/元数据检查，不能证明游戏钩子位置及完整输入流程。出现问题时提供游戏、SKSE 和前置版本，以及 SkyrimTextBridge.log 与 skse64.log；避免附带私人输入内容。

LIMITATIONS / 限制
Keyboard filtering covers downstream ordinary events; editing keys, earlier hooks and independent polling can still activate shortcuts. Some IMEs do not expose IMM candidate lists. Chinese UI only; Japanese/Korean localization remains a possible future direction. Retains the optional native input-state/reset API; no client is required to use it.
热键过滤覆盖后续普通键盘事件，编辑键、较早钩子与独立轮询仍可能触发快捷键。部分输入法不提供 IMM 候选列表。目前界面为中文，日语/韩语本地化仍为未来可能方向。保留可选原生输入状态/重置接口，不要求其他模组接入。

LICENSE AND SOURCE / 许可与源码
Copyright (C) 2026 BlackMesa79. Original code: GPL-3.0-only, without warranty. CommonLibSSE-NG 10.1.0: GPL-3.0-or-later with its additional permissions; original third-party notices remain intact. Full licenses and notices follow in this same file. Corresponding source and build instructions accompany this release in SkyrimTextBridge-0.3.12-beta.1-source.zip. Do not distribute the binary without also making its corresponding source available.
Copyright (C) 2026 BlackMesa79。本项目自身代码采用 GPL-3.0-only，不提供担保；CommonLibSSE-NG 10.1.0 采用 GPL-3.0-or-later 及其附加许可，保留第三方原有声明。全文合并于本文件下方。对应源码及构建说明随本版本提供于 SkyrimTextBridge-0.3.12-beta.1-source.zip；转发二进制时也应提供对应源码。

SKSE Menu Framework is separately installed and not bundled. Its API header is included in the source tree. No restriction is added against modification or reverse engineering for debugging modifications to an LGPL-covered component.
SKSE Menu Framework 另行安装，不随包提供；源码内包含 API 头文件。不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
