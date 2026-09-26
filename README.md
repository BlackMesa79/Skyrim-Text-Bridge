# Skyrim Text Bridge

A Skyrim SKSE plugin for Chinese IME input, explicit English typing, and keyboard hotkey filtering. Includes an adjustable candidate panel and optional SKSE Menu Framework settings.

**支持 Meridian UI（如 Tailor）、Modex、SkyUI 等界面的输入；兼容 Prisma UI。** 前者通过 F8 调用本模组输入。Prisma UI 由框架自身处理输入，本模组不为它提供输入支持，也不接管它的输入法；兼容共存，不影响其正常输入。

**Input support:** Meridian UI (including Tailor), Modex, SkyUI and other compatible text fields. **Coexistence compatibility:** Prisma UI uses its own native input; Text Bridge does not provide IME input for Prisma-based interfaces.

## 使用

1. 安装与游戏匹配的 SKSE64 和 Address Library；支持 Skyrim **1.5.97 / 1.6.1170**。
2. 将发行 ZIP 安装到 MO2，禁用 SimpleIME。更新前退出游戏；个人 `SkyrimTextBridge.user.ini` 会保留。
3. 点击文本框，按 **F8** 开启输入；按 **Ctrl + 空格**切换中英文，再按 F8 关闭。切换语言会取消尚未选定的拼音。
4. 可选安装 SKSE Menu Framework，在默认 F1 菜单的 **Skyrim Text Bridge → 输入法与面板设置**调整开关、快捷键、位置、颜色及背景透明度。

游戏使用窗口化或无边框模式。切换菜单、读档、失焦或 Prisma 接管时退出输入模式。

面板显示当前 Windows 输入法名称，并在切换输入法后刷新（同语言配置切换约一秒内更新）。标题中的中/英文是 Text Bridge 的输入模式，名称行表示当前选中的 Windows 输入法。部分输入法未提供名称时会显示“名称不可用”及可获取的语言信息。

## Compatibility / 兼容性

| 组件 | 接入方式 | 版本检查 |
| --- | --- | --- |
| SkyUI / Modex | **输入支持**：F8 调用本模组，经游戏字符通道输入 | 无模组版本白名单 |
| Prisma UI / Outfit Wheeler | **兼容共存**：使用 Prisma 原生输入，本模组不提供输入支持、不接管 | 仅查询公开 V1 焦点接口以避让，不限制框架发行版本 |
| Meridian UI / Tailor | **输入支持**：F8 调用本模组，经 View/1 与 DOM 提交文字 | 协商 `Meridian.View/1`，不限制框架发行版本 |
| SKSE Menu Framework | 可选设置页 | 检查所需导出接口，不限制发行版本 |
| Skyrim | SKSE / CommonLib 地址与结构 | 严格限制 1.5.97、1.6.1170 |
| SimpleIME | 使用同一输入目标 | 禁止同时启用 |

Meridian 的焦点观察依赖 MSVC x64 View/1 的公开虚表布局；成功协商接口并不意味着所有第三方界面均已实测。已测试的框架包括 Prisma 1.5.1、Meridian 1.5.0 和 Menu Framework 3.14 系列。

用户确认 0.3.4 在当前 1.6.1170 环境运行正常。0.3.5 保留该行为，并移除 Meridian 的发行版本白名单；此调整通过离线协商和 ABI 检查。1.5.97 仍待游戏实测。

## 热键过滤范围

F8 输入模式中，普通键的按下、持续和松开事件不会送入下游游戏输入链；退出后也处理尾随松开。进入模式前已送出的按下仍会收到对应松开，以免按键卡住。

鼠标与退格、Enter、Esc、Tab、方向键、Home/End、Page Up/Down、Delete 保留；选词期间这些编辑键由输入法使用。绑定在编辑键上的其他热键仍可能触发。独立轮询键盘、较早的输入钩子不保证能被拦截。Prisma 原生输入不应用此过滤。

不自动识别所有文本框，不提供通用 Ctrl+A/C/V 支持。部分 IME 不提供 IMM 候选列表；其他键盘布局及死键组合仍待验证。

## Build / 开发

完整构建步骤见 [BUILDING.md](BUILDING.md)。主要源码位于 `src/`、`include/` 和 `assets/`；当前使用的第三方源码/SDK 位于 `extern/`，不上传游戏文件、个人配置或二进制依赖。

```powershell
./scripts/Build-Local.ps1 -CommonLibArchive '<path to matching CommonLib static library>'
./scripts/Package.ps1 -Release
```

发行包不包含 PDB，符号单独打包。SKSE 日志中的 `SkyrimTextBridge.log` 用于排查；不记录输入文字。

## License

Copyright (C) 2026 BlackMesa79. Original Skyrim Text Bridge code is licensed under **GPL-3.0-only**; see [LICENSE](LICENSE). Third-party files retain their original licenses and are not relicensed by this project. See [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).
