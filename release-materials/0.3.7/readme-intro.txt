SKYRIM TEXT BRIDGE 0.3.7
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

INPUT SUPPORT / 输入支持
Meridian UI (including Tailor), Modex and SkyUI: F8 activates Text Bridge to enter text.
Meridian UI（如 Tailor）、Modex、SkyUI：按 F8 调用本模组输入文字。

PRISMA COMPATIBILITY / Prisma 兼容性
Prisma UI and Outfit Wheeler use their own native input. Text Bridge does not provide input support for them and steps aside to preserve their normal input.
Prisma UI 与 Outfit Wheeler 使用自身原生输入。本模组不为其提供输入支持，主动避让，不接管其正常输入。

SETUP / 安装
Install with MO2; disable SimpleIME. Requires Windows, matching SKSE64 and Address Library. DLL runtimes: 1.5.97 / 1.6.1170; user-tested on 1.6.1170, 1.5.97 remains unverified in game. Use windowed or borderless mode.
通过 MO2 安装，禁用 SimpleIME。需要 Windows、对应版本的 SKSE64 与 Address Library。DLL 接受 1.5.97 / 1.6.1170；目前用户实测 1.6.1170，1.5.97 仍待游戏验证。使用窗口化或无边框模式。

Click a supported field → F8 → type/select candidates. Ctrl + Space switches Chinese/English; F8 exits. Switching language cancels uncommitted composition. Turn input mode off before changing fields or resuming gameplay.
点击受支持文本框 → F8 → 输入/选词。Ctrl + 空格切换中英文，F8 退出。切换语言会取消未提交的组合文本。更换文本框或恢复游戏操作前关闭输入模式。

Optional SKSE Menu Framework: F1 → Skyrim Text Bridge → 输入法与面板设置. Customize enabled state, toggle key, position, scale, colors and opacity. Save settings to persist them.
可选 SKSE Menu Framework：F1 → Skyrim Text Bridge → 输入法与面板设置，可调整开关、快捷键、位置、缩放、颜色及透明度。保存后下次启动保留。

LIMITATIONS / 限制
Hotkey filtering covers downstream ordinary keyboard events; editing keys, earlier hooks and independent polling can still trigger hotkeys. Mouse remains available. No universal Ctrl+A/C/V support. Some IMEs expose no IMM candidate list. Name fallback is labeled as keyboard language. Chinese UI only; Japanese/Korean localization is only a possible future direction.
热键过滤针对后续普通键盘事件；编辑键、较早钩子及独立轮询仍可能触发热键。鼠标保留，不提供通用 Ctrl+A/C/V 支持。部分输入法不提供 IMM 候选列表；名称回退时标注为键盘语言。目前界面为中文，日语/韩语本地化仅为未来可能的方向。

LICENSE / 许可
Copyright (C) 2026 BlackMesa79. Original Skyrim Text Bridge code: GPL-3.0-only, without warranty. Complete GPL text and third-party notices follow in this same file. Third-party files are not relicensed.
Copyright (C) 2026 BlackMesa79。本项目自身代码采用 GPL-3.0-only，不提供担保。GPL 全文及第三方许可均合并在本文件下方；第三方文件不重新授权为本项目协议。

Corresponding source, build instructions and unmodified SDK headers:
对应源码、构建说明和未修改的 SDK 头文件：
https://github.com/BlackMesa79/Skyrim-Text-Bridge/tree/v0.3.7

The optional SKSE Menu Framework runtime is separately installed, not bundled. Its API header is available in the source tree. No restriction is added against modification or reverse engineering for debugging modifications to an LGPL-covered component.
SKSE Menu Framework 运行时另行安装，不随包提供；其 API 头文件可在源码中取得。不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
