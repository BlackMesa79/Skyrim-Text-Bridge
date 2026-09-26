# Skyrim Text Bridge - Chinese IME Support

## Overview / 简介

Type Chinese directly into supported Skyrim text fields. Skyrim Text Bridge provides an F8 input mode, an IME candidate panel, an explicit English mode and keyboard hotkey filtering, with optional in-game appearance settings.

在受支持的天际文本框中直接输入中文。Skyrim Text Bridge 提供 F8 输入模式、输入法候选面板、独立英文模式和键盘热键过滤，并可通过游戏内设置调整面板外观。

This mod currently focuses on Chinese-speaking players. Its panel and settings are currently in Chinese; this description is bilingual. Japanese and Korean localization may be considered in the future, but neither localization nor dedicated Japanese/Korean IME compatibility is included or promised in this release.

本模组目前主要面向中文玩家，面板和设置界面使用中文，本详情页采用英中对照。未来可能考虑日语、韩语本地化，但本版本尚未提供相应本地化，也不宣称已专门适配日语或韩语输入法。

## Input support and compatibility / 输入支持与兼容性

**Input support — Meridian UI (including Tailor), Modex and SkyUI.** In these supported interfaces, pressing F8 activates Text Bridge and uses this mod to enter text. Other compatible text fields may also work; support for every mod or interface is not guaranteed.

**输入支持——Meridian UI（包括 Tailor）、Modex 和 SkyUI。** 在这些已支持的界面中，按 F8 会实际调用 Text Bridge 的功能输入文字。其他兼容文本框也可能可用，但不保证所有模组和界面均受支持。

**Coexistence compatibility — Prisma UI and Prisma-based interfaces such as Outfit Wheeler.** Prisma uses its own native input system. Text Bridge does not provide IME input for these interfaces and does not take over their input; it steps aside so Prisma can continue handling input normally. This is compatibility, not Text Bridge input support.

**兼容共存——Prisma UI，以及 Outfit Wheeler 等基于 Prisma 的界面。** Prisma 使用自身原生输入机制，Text Bridge 不为这些界面提供输入法支持，也不会接管其输入，而是主动避让，让 Prisma 正常处理输入。这属于兼容共存，不属于 Text Bridge 的输入支持。

**SimpleIME must be disabled.** Do not enable SimpleIME and Text Bridge together. SKSE Menu Framework is optional: it provides Text Bridge's settings page but is not required for basic input.

**请禁用 SimpleIME。** 不要同时启用 SimpleIME 与 Text Bridge。SKSE Menu Framework 为可选组件，用于提供本模组的设置页面，基础输入功能不依赖它。

## Features / 功能

The panel shows composition text and candidates supplied by the IME. It also displays the active input method's name. If Windows exposes only language information, the panel labels it as the keyboard language; if neither is available, it displays an unknown status. The selected input method and Text Bridge's Chinese/English mode are shown separately.

面板显示输入法提供的拼音组合文本与候选词，同时显示当前输入法名称。如果 Windows 只提供语言信息，则明确显示为“键盘语言”；两者都未取得时显示未知。系统选中的输入法与 Text Bridge 的中英文模式分别展示。

While F8 mode is active, ordinary keyboard presses, held events and releases are filtered from the downstream game input chain to reduce accidental hotkey activation. Mouse input and common editing keys remain available. This is not a universal blocker: hotkeys bound to editing keys, independent keyboard polling or earlier input hooks can still react. Universal Ctrl+A/C/V support is not provided.

F8 模式开启期间，会过滤发往后续游戏输入链的普通键按下、持续按住和松开事件，减少误触其他热键。鼠标和常用编辑键仍然保留。这并非万能的热键屏蔽器：绑定在编辑键上的热键、独立读取键盘状态的模组或更早的输入钩子仍可能响应。目前不提供通用的 Ctrl+A/C/V 编辑组合键支持。

With SKSE Menu Framework, you can change the mod's enabled state, toggle key, panel position, scale, colors and background opacity. Text remains readable when the background is transparent. Save settings to keep them between game sessions.

安装 SKSE Menu Framework 后，可调整模组开关、启用快捷键、面板位置、缩放、颜色及背景不透明度。背景透明时文字仍保持清晰。点击保存，可在下次启动游戏时保留设置。

## Requirements / 前置要求

Requires Windows, SKSE64 and the Address Library matching your Skyrim runtime. The DLL accepts Skyrim SE 1.5.97 and AE 1.6.1170; current user validation is on 1.6.1170, while 1.5.97 still needs in-game verification. Use windowed or borderless mode and an installed Chinese IME. Install the relevant UI framework separately when using its dependent mods.

需要 Windows、与游戏运行时匹配的 SKSE64 和 Address Library。DLL 接受 Skyrim SE 1.5.97 与 AE 1.6.1170；当前用户实测环境为 1.6.1170，1.5.97 仍待游戏内验证。请使用窗口化或无边框模式，并安装中文输入法。使用依赖界面框架的模组时，请另行安装其所需框架。

## Installation and use / 安装与使用

Install the release ZIP with MO2 or your mod manager. It contains only the plugin DLL, the default INI and one readme.txt with usage and license information. Exit Skyrim before updating, enable only one version of Text Bridge and disable SimpleIME. Existing personal settings are not included in the archive and should be preserved.

通过 MO2 或其他模组管理器安装发布 ZIP。包内只有插件 DLL、默认 INI 和一个包含使用及许可说明的 readme.txt。更新前退出游戏，只启用一个 Text Bridge 版本，并禁用 SimpleIME。发布包不包含个人配置，更新时应保留已有设置。

Click a supported text field, then press F8 to enter input mode. Type and select candidates normally. Press Ctrl + Space to switch between Text Bridge's Chinese and English modes, and press F8 again to leave. Switching language cancels uncommitted composition, so choose your candidate first. Close input mode before moving to another text field or returning to gameplay.

点击受支持的文本框，按 F8 开启输入模式，正常打字并选择候选词。按 Ctrl + 空格切换 Text Bridge 的中文和英文模式，再按 F8 退出。切换语言会取消尚未提交的组合文本，请先选词。在切换文本框或返回正常游戏操作前，请关闭输入模式。

For settings, open SKSE Menu Framework (F1 by default) and select Skyrim Text Bridge → 输入法与面板设置. Opening settings pauses input mode. Prisma interfaces should continue using their own native input rather than Text Bridge's F8 mode.

调整设置时，打开 SKSE Menu Framework（默认 F1），进入 Skyrim Text Bridge → 输入法与面板设置。打开设置时会暂停输入模式。Prisma 界面请继续使用其自身的原生输入机制，不使用 Text Bridge 的 F8 模式。

## Known limitations / 已知限制

Some modern IMEs do not expose candidate lists through IMM; in that case, the panel cannot display those candidates. Name detection depends on the information Windows makes available. The mod does not automatically identify every text field, does not support password fields and does not fix Prisma's own candidate-window issues. Alternative keyboard layouts and dead-key combinations have not been fully tested.

部分现代输入法不会通过 IMM 提供候选列表，此时面板无法展示这些候选词。名称识别取决于 Windows 提供的信息。本模组不会自动识别所有文本框，不支持密码字段，也不修复 Prisma 自身的候选窗问题。其他键盘布局与死键组合尚未充分测试。

## Source, license and feedback / 源码、许可与反馈

Original Text Bridge code is licensed under GPL-3.0-only. Third-party dependencies retain their own licenses; complete notices are included in readme.txt. Source: https://github.com/BlackMesa79/Skyrim-Text-Bridge . Please include your Skyrim runtime, IME name, related mod versions, reproduction steps and SkyrimTextBridge.log when reporting problems. Entered text is not logged.

Text Bridge 自身代码采用 GPL-3.0-only 协议，第三方依赖保留原有许可，完整声明见 readme.txt。源码：https://github.com/BlackMesa79/Skyrim-Text-Bridge 。反馈问题时，请提供游戏运行时、输入法名称、相关模组版本、复现步骤和 SkyrimTextBridge.log；日志不会记录输入文字。
