# Skyrim Text Bridge

**Experimental branch — 0.3.11-beta.1 / 实验分支。** Adds exact Skyrim **1.7.104** admission using CommonLibSSE-NG **10.1.0** and version-aware character queues. Existing 1.5.97 / 1.6.1170 admission is retained. This build has passed offline checks, but none of these runtimes has been game-tested with this new binary yet. The user-tested 0.3.10 stable build remains separate. See [beta notes](release-materials/0.3.11-beta.1/TESTING.md).

**1.7.104 requires SKSE 2.3.1, its matching Address Library and a working nonexclusive keyboard patch (Media Keys Fix SKSE or equivalent).** The locally installed Media Keys Fix 1.0.2 advertises 1.7.99; its 1.7.104 operation is still unverified. Do not assume updating Text Bridge also updates its prerequisites. 本实验包先供本地回归测试，待用户确认后再公开发布测试版；请勿同时启用正式版和实验版。

0.3.10 adds optional automatic activation and a clean Esc exit that returns game input to English. Media Keys Fix SKSE is required for the normal setup. An optional native input-state/reset API is retained for FavoriteWheel; that separate integration still requires its own validation.

A Skyrim SKSE plugin for Chinese IME input, explicit English typing, and keyboard hotkey filtering. Includes an adjustable candidate panel and optional SKSE Menu Framework settings.

**支持 Meridian UI（如 Tailor）、Modex、SkyUI 等界面的输入；兼容 Prisma UI。** 前者通过 F8 调用本模组输入。Prisma UI 由框架自身处理输入，本模组不为它提供输入支持，也不接管它的输入法；兼容共存，不影响其正常输入。

**Input support:** Meridian UI (including Tailor), Modex, SkyUI and other compatible text fields. **Coexistence compatibility:** Prisma UI uses its own native input; Text Bridge does not provide IME input for Prisma-based interfaces.

## 使用

1. 安装与游戏匹配的 SKSE64、Address Library 和 Media Keys Fix SKSE（或等效非独占键盘补丁）；实验 DLL 接受 Skyrim **1.5.97 / 1.6.1170 / 1.7.104**，实测状态见上方说明。
2. 将发行 ZIP 安装到 MO2，禁用 SimpleIME。更新前退出游戏；个人 `SkyrimTextBridge.user.ini` 会保留。
3. 点击文本框，按 **F8** 开启输入；按 **Ctrl + 空格**切换中英文，再按 F8 关闭。切换语言会取消尚未选定的拼音。
4. 可选安装 SKSE Menu Framework，在默认 F1 菜单的 **Skyrim Text Bridge → 输入法与面板设置**调整开关、快捷键、自动开启、位置、颜色及背景透明度。

自动开启默认关闭。在设置页勾选“文本框聚焦时自动开启”并保存，或在 `SkyrimTextBridge.user.ini` 的 `[Input]` 下设置 `AutoEnable=1`。首次接入范围为 SkyUI 的物品、魔法、容器、交易菜单搜索框，以及 Meridian/Tailor 的可编辑文本框。Modex 保留手动 F8，Prisma 始终避让。按自定义快捷键手动关闭后，当前焦点暂不自动重开；离开再聚焦后恢复。自动开启的会话失焦后结束，换框时清空旧会话的未提交内容。

Automatic activation is optional and off by default. It covers SkyUI item-menu searches and Meridian/Tailor text fields. Manual off is respected until focus leaves and returns. Modex remains manual; Prisma retains native input. Automatic activation was user-tested in SkyUI and Tailor on 0.3.9; the user has also confirmed the improved 0.3.10 Esc exit in the current 1.6.1170 setup.

输入模式内，**Esc 先取消正在选词的拼音，输入完成后退出模式**。退出的按下和松开不会传入游戏，下次 Esc 恢复界面的正常操作。若刚选定的文字仍待提交，该次 Esc 等待文字送达，再按一次可退出。普通退出、F8 关闭及菜单重置会关闭恢复后的游戏 IME 上下文；下次开启本模组重新启用输入法，所选键盘布局保持不变。Prisma 或其他框架接管时不修改其上下文。退出时仍按住的移动键需松开后重新按下。

游戏使用窗口化或无边框模式。切换菜单、读档、失焦或 Prisma 接管时退出输入模式。

面板显示当前 Windows 输入法名称，并在切换输入法后刷新（同语言配置切换约一秒内更新）。标题中的中/英文是 Text Bridge 的输入模式。读到具体名称时显示“当前输入法：……”；只读到语言信息时显示“键盘语言：……”，不会将语言误标为输入法名称；两者都未取得时显示“当前输入法：未知”。

## Compatibility / 兼容性

| 组件 | 接入方式 | 版本检查 |
| --- | --- | --- |
| SkyUI / Modex | **输入支持**：F8 调用本模组，经游戏字符通道输入；SkyUI 指定菜单搜索框可选自动开启，Modex 手动 | 无模组版本白名单 |
| Prisma UI / Outfit Wheeler | **兼容共存**：使用 Prisma 原生输入，本模组不提供输入支持、不接管 | 仅查询公开 V1 焦点接口以避让，不限制框架发行版本 |
| Meridian UI / Tailor | **输入支持**：F8 或可选自动开启，经 View/1 与 DOM 提交文字 | 协商 `Meridian.View/1`，不限制框架发行版本 |
| SKSE Menu Framework | 可选设置页 | 检查所需导出接口，不限制发行版本 |
| Skyrim | SKSE / CommonLib 地址与结构 | 实验分支严格限制 1.5.97、1.6.1170、1.7.104；未放行其他 1.7.x |
| Media Keys Fix SKSE | 实际输入前置，提供非独占键盘环境；等效补丁可替代 | 不静态链接其 DLL |
| SimpleIME | 使用同一输入目标 | 禁止同时启用 |

Meridian 的焦点观察依赖 MSVC x64 View/1 的公开虚表布局；成功协商接口并不意味着所有第三方界面均已实测。已测试的框架包括 Prisma 1.5.1、Meridian 1.5.0 和 Menu Framework 3.14 系列。

用户已确认 0.3.7 在当前 1.6.1170 环境运行正常，并批准该版本发布。0.3.9 的自动开启已获用户在 SkyUI 与 Tailor 中确认；用户也已确认 0.3.10 的 Esc 退出优化有效并批准发布；1.5.97 仍待游戏实测。

## 热键过滤范围

F8 输入模式中，普通键的按下、持续和松开事件不会送入下游游戏输入链；退出后也处理尾随松开。进入模式前已送出的按下仍会收到对应松开，以免按键卡住。

鼠标与退格、Enter、Tab、方向键、Home/End、Page Up/Down、Delete 保留；选词期间这些编辑键由输入法使用。绑定在编辑键上的其他热键仍可能触发。独立轮询键盘、较早的输入钩子不保证能被拦截。Prisma 原生输入不应用此过滤。

自动开启仅覆盖上述焦点信号明确的界面，不自动识别所有文本框，并排除密码、只读和禁用字段。非选词期间允许 Ctrl+A/C/X/V 通过，具体编辑操作由目标界面实现。部分 IME 不提供 IMM 候选列表；其他键盘布局及死键组合仍待验证。

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
