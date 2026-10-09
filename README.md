# Skyrim Text Bridge

**Local test update — 0.3.12-beta.5 / 本地测试更新。** Fixes beta.4 SkyUI automatic activation by correcting CustomRendering and passive overlay ownership. Routes optional automatic input by UI framework. Scaleform uses actual focused editable fields across registered open input menus, with no menu-name whitelist. Meridian retains its framework-wide field detection; Prisma retains native input. Independent ImGui interfaces such as Modex remain manual. Keeps beta.3 Esc/TSF fixes and the ImGui HUD. **SKSE Menu Framework and a configured Chinese font are required.** See [test notes](release-materials/0.3.12-beta.5/TESTING.md) and [framework design](FRAMEWORK-INPUT.md); the new binary needs game testing. Stable 0.3.11 and earlier test ZIPs remain available.

**0.3.11 / 正式打包版本。** Uses CommonLibSSE-NG **10.1.0**, V5 Address Library support and version-aware character queues. Exact runtime whitelist: **1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104**. The user confirmed normal local **1.6.1170** operation and approved normal-release packaging on 2026-10-05. 1.5.97 and both 1.7 runtimes still await game testing. See [validation notes](VALIDATION-0.3.11.md).

**1.7.104 requires SKSE 2.3.1, its matching Address Library and a working nonexclusive keyboard patch (Media Keys Fix SKSE or equivalent).** The locally installed Media Keys Fix 1.0.2 advertises 1.7.99; its 1.7.104 operation is still unverified. Do not assume updating Text Bridge also updates its prerequisites. 已按用户要求打包为 0.3.11 正式版；1.7.x 仍待玩家实测反馈。请勿同时启用多个 Text Bridge 版本。

0.3.10 adds optional automatic activation and a clean Esc exit that returns game input to English. Media Keys Fix SKSE is required for the normal setup. An optional native input-state/reset API is retained for FavoriteWheel; that separate integration still requires its own validation.

A Skyrim SKSE plugin for Chinese IME input, explicit English typing, and keyboard hotkey filtering. Includes an adjustable candidate panel and SKSE Menu Framework HUD rendering and settings.

**支持 Meridian UI（如 Tailor）、Modex、SkyUI 等界面的输入；兼容 Prisma UI。** 前者通过 F8 调用本模组输入。Prisma UI 由框架自身处理输入，本模组不为它提供输入支持，也不接管它的输入法；兼容共存，不影响其正常输入。

**Input support:** Meridian UI (including Tailor), Modex, SkyUI and other compatible text fields. **Coexistence compatibility:** Prisma UI uses its own native input; Text Bridge does not provide IME input for Prisma-based interfaces.

## 使用

1. 安装与游戏匹配的 SKSE64、Address Library 和 Media Keys Fix SKSE（或等效非独占键盘补丁）；DLL 接受 Skyrim **1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104**，实测状态见上方说明。
2. 将发行 ZIP 安装到 MO2，禁用 SimpleIME。更新前退出游戏；个人 `SkyrimTextBridge.user.ini` 会保留。
3. 点击文本框，按 **F8** 开启输入；按 **Ctrl + 空格**切换中英文，再按 F8 关闭。切换语言会取消尚未选定的拼音。
4. 安装匹配游戏的 SKSE Menu Framework，并配置中文字体（EnableChinese=true 和有效 PrimaryFont）；在默认 F1 菜单的 **Skyrim Text Bridge → 输入法与面板设置**调整开关、快捷键、自动开启、位置、颜色及背景透明度。

自动开启默认关闭。在设置页勾选“文本框聚焦时自动开启”并保存，或在 `SkyrimTextBridge.user.ini` 的 `[Input]` 下设置 `AutoEnable=1`。按 UI 框架识别：Scaleform 不再限定菜单名称，而是检查已打开的最上层输入菜单、实际可编辑焦点与可见性，覆盖符合条件的 SkyUI、RaceMenu 及其他 Scaleform 界面；Meridian/Tailor 沿用框架级文本框检测。Modex 等独立 ImGui 界面保留手动 F8，Prisma 始终避让。按自定义快捷键手动关闭后，当前焦点暂不自动重开；离开再聚焦后恢复。自动开启的会话失焦后结束，换框时清空旧会话的未提交内容。新界面的文字提交仍需实测，识别焦点不等于所有界面已经完整兼容。

Automatic activation is optional and off by default. Scaleform now discovers focused editable fields by framework capability, not menu names; Meridian/Tailor keeps its framework field observer. Upper dialogs/custom input owners prevent activation of stale fields underneath. Manual off is respected until focus leaves and returns. Modex and other independent ImGui interfaces remain manual; Prisma retains native input. Earlier automatic activation was user-tested in SkyUI and Tailor; generic Scaleform detection and new fields still need game testing.

输入模式内，**Esc 先取消正在选词的拼音，输入完成后退出模式**。退出的按下和松开不会传入游戏，下次 Esc 恢复界面的正常操作。若刚选定的文字仍待提交，该次 Esc 等待文字送达，再按一次可退出。普通退出、F8 关闭及菜单重置会关闭恢复后的游戏 IME 上下文；下次开启本模组重新启用输入法，所选键盘布局保持不变。Prisma 或其他框架接管时不修改其上下文。退出时仍按住的移动键需松开后重新按下。

候选面板已改为游戏内 ImGui 绘制，不再创建桌面悬浮窗口。目标包括窗口化、无边框及独占全屏；真正独占全屏、Alt+Tab 与渲染模组组合仍需游戏验证。面板不抢占焦点、不打开框架窗口、不暂停游戏；Media Keys Fix 的非独占键盘访问仍为输入前置。框架须加载中文字体，缺字可检查日志与框架字体配置。切换菜单、读档、失焦或 Prisma 接管时退出输入模式。

面板显示当前 Windows 输入法名称，并在切换输入法后刷新（同语言配置切换约一秒内更新）。标题中的中/英文是 Text Bridge 的输入模式。读到具体名称时显示“当前输入法：……”；只读到语言信息时显示“键盘语言：……”，不会将语言误标为输入法名称；两者都未取得时显示“当前输入法：未知”。

## Compatibility / 兼容性

| 组件 | 接入方式 | 版本检查 |
| --- | --- | --- |
| Scaleform（SkyUI、RaceMenu 等） | **输入支持**：经游戏字符通道输入；有效可编辑焦点可选自动开启，未识别字段手动 F8 | 无菜单名称白名单；新增字段的文字送达需实测 |
| Modex / 独立 ImGui | Modex **输入支持**：F8 手动，经游戏字符通道输入；保留 Esc/关闭清理 | Modex 3.0.2 相关公开 IMenu 路径未变；其他独立 ImGui 不自动认定为兼容 |
| Prisma UI / Outfit Wheeler | **兼容共存**：使用 Prisma 原生输入，本模组不提供输入支持、不接管 | 仅查询公开 V1 焦点接口以避让，不限制框架发行版本 |
| Meridian UI / Tailor | **输入支持**：F8 或可选自动开启，经 View/1 与 DOM 提交文字 | 协商 `Meridian.View/1`，不限制框架发行版本 |
| SKSE Menu Framework | 必需：ImGui HUD 面板及设置页，需要中文字体配置 | 检查所需 HUD/ImGui 导出接口；本地检查 3.18，需框架自身匹配游戏 |
| Skyrim | SKSE / CommonLib 地址与结构 | 严格限制 1.5.97、1.6.1170、1.7.99、1.7.104；未放行其他 1.7.x |
| Media Keys Fix SKSE | 实际输入前置，提供非独占键盘环境；等效补丁可替代 | 不静态链接其 DLL |
| SimpleIME | 使用同一输入目标 | 禁止同时启用 |

Meridian 的焦点观察依赖 MSVC x64 View/1 的公开虚表布局；成功协商接口并不意味着所有第三方界面均已实测。已测试的框架包括 Prisma 1.5.1、Meridian 1.5.0 和 Menu Framework 3.14 系列。

用户已确认 0.3.7 在当前 1.6.1170 环境运行正常，并批准该版本发布。0.3.9 的自动开启已获用户在 SkyUI 与 Tailor 中确认；用户也已确认 0.3.10 的 Esc 退出优化有效并批准发布；1.5.97 仍待游戏实测。

## 热键过滤范围

F8 输入模式中，普通键的按下、持续和松开事件不会送入下游游戏输入链；退出后也处理尾随松开。进入模式前已送出的按下仍会收到对应松开，以免按键卡住。

鼠标与退格、Enter、Tab、方向键、Home/End、Page Up/Down、Delete 保留；选词期间这些编辑键由输入法使用。绑定在编辑键上的其他热键仍可能触发。独立轮询键盘、较早的输入钩子不保证能被拦截。Prisma 原生输入不应用此过滤。

自动开启要求框架提供可靠焦点信号，不自动识别所有文本框，排除密码、只读、隐藏和禁用字段。Scaleform 同时要求游戏文本输入许可，不能仅凭残留的许可计数开启；上层输入归属者缺少有效字段时不回退到底下的字段。非选词期间允许 Ctrl+A/C/X/V 通过，具体编辑操作由目标界面实现。部分 IME 不提供 IMM 候选列表；其他键盘布局及死键组合仍待验证。

## Build / 开发

完整构建步骤见 [BUILDING.md](BUILDING.md)。主要源码位于 `src/`、`include/` 和 `assets/`；当前使用的第三方源码/SDK 位于 `extern/`，不上传游戏文件、个人配置或二进制依赖。

```powershell
./scripts/Build-Local.ps1 -CommonLibArchive '<path to matching CommonLib static library>'
./scripts/Package.ps1 -Release
```

发行包不包含 PDB，符号单独打包。SKSE 日志中的 `SkyrimTextBridge.log` 用于排查；不记录输入文字。

正式安装 ZIP 仅含 DLL、默认 INI 与一个 `readme.txt`，使用说明和完整许可文本合并在该文件中。逐段英中对照详情页、简介及 SVG 封面位于 `release-materials/0.3.7/`，不占用安装包。

## License

Copyright (C) 2026 BlackMesa79. Original Skyrim Text Bridge code is licensed under **GPL-3.0-only**; see [LICENSE](LICENSE). Third-party files retain their original licenses and are not relicensed by this project. See [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).
