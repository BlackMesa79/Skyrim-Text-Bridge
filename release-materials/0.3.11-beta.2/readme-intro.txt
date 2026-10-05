SKYRIM TEXT BRIDGE 0.3.11-beta.2 — PRIVATE RUNTIME TEST
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

STATUS / 状态
An isolated experimental build for exact Skyrim 1.7.99 and 1.7.104 support. Not yet approved for a public beta. The user-tested 0.3.10 stable build is unchanged. This new binary has passed offline checks only; in-game validation is pending on all four admitted runtimes.
独立实验构建，目标为 Skyrim 1.7.99 与 1.7.104。目前先供本地测试，尚未批准公开发布测试版。用户实测通过的 0.3.10 正式版保持不变。新 DLL 仅通过离线检查，接受的四个游戏版本均仍需用此 DLL 进行游戏验证。

Previous beta.1 was user-tested successfully on 1.6.1170. Beta.2 adds 1.7.99 admission and new offline address checks; this updated binary still needs a fresh game test.
前一版 beta.1 已由用户在 1.6.1170 实测正常。beta.2 新增 1.7.99 放行及离线地址检查，新 DLL 仍需重新进行游戏测试。

CHANGES / 更新
Updates CommonLibSSE-NG to 10.1.0, handles V5 Address Library files, and uses version-aware character-pool and input-queue accessors for 1.7.99 / 1.7.104. Exact runtime admission: 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104; other versions are rejected. No blanket 1.7.x compatibility claim.
更新 CommonLibSSE-NG 至 10.1.0，处理 V5 地址库，并通过对应版本的字符池和输入队列接口适配 1.7.99 / 1.7.104。精确放行 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104，其他版本拒绝加载，不声明所有 1.7.x 都兼容。

REQUIREMENTS / 前置
Windows, matching SKSE64 and Address Library, and Media Keys Fix SKSE or an equivalent nonexclusive keyboard patch that works on your runtime. Skyrim 1.7.104 uses SKSE 2.3.1 and its matching Address Library. The locally installed Media Keys Fix 1.0.2 lists 1.7.99; its 1.7.104 operation has not been verified. Updating Text Bridge does not update other mods. Optional SKSE Menu Framework and UI frameworks also need their own compatible builds.
需要 Windows、匹配游戏的 SKSE64 和 Address Library，以及能在该游戏版本上工作的 Media Keys Fix SKSE 或等效非独占键盘补丁。1.7.104 对应 SKSE 2.3.1 及匹配的地址库。本地 Media Keys Fix 1.0.2 的说明列出 1.7.99，其在 1.7.104 上的工作状态尚未验证。Text Bridge 更新不会替其他模组完成适配；可选的 SKSE Menu Framework 和 UI 框架也需各自匹配版本。

INSTALL AND ROLLBACK / 安装与回退
Exit Skyrim and install this package as a separate MO2 mod. Disable stable Text Bridge and SimpleIME before enabling the test package; never enable both Text Bridge versions together. Use windowed or borderless mode. Existing SkyrimTextBridge.user.ini settings remain compatible and may be copied to the test mod. Roll back by disabling the test mod and enabling 0.3.10 again; no save edits are needed.
退出游戏，将本包作为独立 MO2 模组安装。开启测试版前禁用正式版 Text Bridge 与 SimpleIME，不要同时开启两个 Text Bridge。使用窗口化或无边框模式。旧 SkyrimTextBridge.user.ini 配置可复制至测试模组沿用。回退时禁用测试版、重新开启 0.3.10 即可，无需修改存档。

INPUT SUPPORT / 输入支持
Meridian UI (including Tailor), Modex and SkyUI: focus a supported field and press F8 to use Text Bridge. Ctrl + Space switches Chinese/English. F8 exits. Esc cancels ongoing composition first; after completed input it exits and closes the restored game IME context so movement keys do not become pinyin. If committed characters are still queued, wait for delivery and press Esc again.
Meridian UI（如 Tailor）、Modex、SkyUI：聚焦受支持文本框，按 F8 调用 Text Bridge。Ctrl + 空格切换中英文，F8 关闭。Esc 先取消选词中的拼音；输入完成后退出并关闭恢复后的游戏 IME 上下文，避免移动键变成拼音。如果文字尚在提交队列中，请等送达后再按一次 Esc。

PRISMA COMPATIBILITY / Prisma 兼容性
Prisma UI and Outfit Wheeler use their own native input. Text Bridge yields to Prisma and does not provide IME support for Prisma-based interfaces. This compatibility behavior still needs regression testing after the dependency update.
Prisma UI 与 Outfit Wheeler 使用自身原生输入。Text Bridge 主动避让，不为 Prisma 界面提供输入法支持。依赖更新后仍需回归验证这种共存行为。

SETTINGS / 设置
Optional SKSE Menu Framework: F1 -> Skyrim Text Bridge -> 输入法与面板设置. Customize enabled state, hotkey, panel position, size, colors and opacity. Save to persist settings. Optional automatic activation is off by default; it covers SkyUI item-menu searches and Meridian/Tailor editable fields. Modex stays manual; Prisma retains native input. Manual off suppresses reopening until focus leaves and returns.
可选 SKSE Menu Framework：F1 -> Skyrim Text Bridge -> 输入法与面板设置。可调整开关、热键、面板位置、大小、颜色及透明度，保存后保留。自动开启默认关闭，覆盖 SkyUI 物品菜单搜索框及 Meridian/Tailor 可编辑文本框；Modex 手动开启，Prisma 使用原生输入。手动关闭后，离开文本框再重新聚焦才会自动重开。

TESTING / 测试
First check loading, settings registration, Chinese candidates, English typing, Esc exit/movement, auto activation, and Prisma coexistence on the existing 1.6.1170 setup. Then repeat on 1.7.99 and 1.7.104 with matching prerequisites. Offline checks include 61 core, 43 direct IME, and 78 runtime queue/address assertions plus DOM/API/settings/metadata checks. These do not prove executable hook correctness or end-to-end game input. For a failure, provide the game/SKSE/prerequisite versions and SkyrimTextBridge.log plus skse64.log; avoid including private typed content.
先在现有 1.6.1170 环境回归检查加载、设置页注册、中文候选、英文输入、Esc 退出及移动、自动开启和 Prisma 共存，再在前置匹配的 1.7.99 与 1.7.104 上重复。离线检查包括 61 项核心、43 项直接 IME、78 项运行时队列/地址断言以及 DOM/API/设置/元数据检查，不能证明游戏钩子位置及完整输入流程。出现问题时提供游戏、SKSE 和前置版本，以及 SkyrimTextBridge.log 与 skse64.log；避免附带私人输入内容。

LIMITATIONS / 限制
Keyboard filtering covers downstream ordinary events; editing keys, earlier hooks and independent polling can still activate shortcuts. Some IMEs do not expose IMM candidate lists. Chinese UI only; Japanese/Korean localization remains a possible future direction. Retains the optional native input-state/reset API; no client is required to use it.
热键过滤覆盖后续普通键盘事件，编辑键、较早钩子与独立轮询仍可能触发快捷键。部分输入法不提供 IMM 候选列表。目前界面为中文，日语/韩语本地化仍为未来可能方向。保留可选原生输入状态/重置接口，不要求其他模组接入。

LICENSE AND SOURCE / 许可与源码
Copyright (C) 2026 BlackMesa79. Original code: GPL-3.0-only, without warranty. CommonLibSSE-NG 10.1.0: GPL-3.0-or-later with its additional permissions; original third-party notices remain intact. Full licenses and notices follow in this same file. Corresponding source and build instructions accompany this test package in SkyrimTextBridge-0.3.11-beta.2-source.zip. Do not distribute the binary without also making its corresponding source available.
Copyright (C) 2026 BlackMesa79。本项目自身代码采用 GPL-3.0-only，不提供担保；CommonLibSSE-NG 10.1.0 采用 GPL-3.0-or-later 及其附加许可，保留第三方原有声明。全文合并于本文件下方。对应源码及构建说明随测试包提供于 SkyrimTextBridge-0.3.11-beta.2-source.zip；转发二进制时也应提供对应源码。

SKSE Menu Framework is separately installed and not bundled. Its API header is included in the source tree. No restriction is added against modification or reverse engineering for debugging modifications to an LGPL-covered component.
SKSE Menu Framework 另行安装，不随包提供；源码内包含 API 头文件。不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
