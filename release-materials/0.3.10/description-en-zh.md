# Skyrim Text Bridge - Chinese IME Support

## Type naturally. Stay in Skyrim. / 在天际，自然输入

Type Chinese directly into supported Skyrim text fields. Skyrim Text Bridge provides an **F8** input mode, a candidate panel, English typing and keyboard hotkey filtering. Optional automatic activation and in-game settings make everyday input easier.

在受支持的天际文本框中直接输入中文。Skyrim Text Bridge 提供 **F8** 输入模式、候选面板、英文输入和键盘热键过滤，并通过可选自动开启及游戏内设置，让日常输入更方便。

The panel and settings currently use Chinese. This description is bilingual. Japanese and Korean localization may be considered later; dedicated Japanese/Korean IME compatibility has not been validated.

面板和设置界面目前使用中文，本详情页采用英中对照。未来可能考虑日语、韩语本地化；目前尚未专门验证日语或韩语输入法兼容性。

## What's new in 0.3.10 / 0.3.10 更新

**Optional automatic activation, off by default.** Enable it to start input when a supported SkyUI search field or Meridian/Tailor text field gains focus. Manual F8 remains available. Turning input off with F8 or Esc keeps it off for the current focus; leaving and refocusing the field rearms automatic activation.

**新增可选自动开启，默认关闭。** 启用后，受支持的 SkyUI 搜索框及 Meridian/Tailor 文本框获得焦点时会自动进入输入模式。F8 手动开关仍然保留；通过 F8 或 Esc 手动退出后，当前焦点不会立即自动重开，离开并重新聚焦后恢复。

**Esc exits cleanly.** After finishing input, press Esc to leave Text Bridge. Normal exit returns the game input context to English input, preventing movement keys from continuing to compose pinyin. The next Text Bridge session re-enables IME input. Your selected Windows input method is retained.

**Esc 退出更顺畅。** 输入完成后按 Esc 退出 Text Bridge。普通退出会让游戏输入上下文回到英文输入状态，避免移动键继续组合拼音；下次进入 Text Bridge 会重新开启输入法，保留 Windows 当前选中的输入法。

**Editing shortcuts and dependency clarification.** Ctrl+A/C/X/V can pass through outside composition; the target UI decides which editing operations it supports. **Media Keys Fix SKSE is now explicitly listed as a required dependency.** Prisma UI is not required to use Text Bridge.

**完善编辑快捷键与依赖说明。** 非选词期间允许 Ctrl+A/C/X/V 通过，实际编辑操作由目标界面决定。现已明确列出必需前置 **Media Keys Fix SKSE**；使用 Text Bridge 不需要安装 Prisma UI。

## Input support & compatibility / 输入支持与兼容性

**Input support — Meridian UI (including Tailor), Modex and SkyUI.** In these interfaces, F8 activates Text Bridge to enter text. Other compatible text fields may also work, but support for every interface is not guaranteed.

**输入支持——Meridian UI（包括 Tailor）、Modex 和 SkyUI。** 在这些界面中，F8 会实际调用 Text Bridge 输入文字。其他兼容文本框也可能可用，但不保证所有界面均受支持。

**Automatic activation — selected SkyUI searches and Meridian/Tailor fields.** SkyUI coverage includes searches in Inventory, Magic, Container and Barter menus. Meridian coverage includes supported editable text fields. Automatic activation excludes readonly, disabled and password fields. Modex remains manual.

**自动开启——指定 SkyUI 搜索框及 Meridian/Tailor 文本框。** SkyUI 范围包括物品、魔法、容器和交易菜单的搜索框；Meridian 范围包括受支持的可编辑文本框。自动开启会排除只读、禁用和密码字段。Modex 保持手动开启。

**Coexistence compatibility — Prisma UI and Prisma-based interfaces such as Outfit Wheeler.** Prisma handles its own native input. Text Bridge steps aside and does not provide or take over IME input in these interfaces. It also does not fix Prisma's own candidate-window issues.

**兼容共存——Prisma UI，以及 Outfit Wheeler 等基于 Prisma 的界面。** Prisma 自行处理原生输入，Text Bridge 主动避让，不为这些界面提供或接管输入法功能，也不修复 Prisma 自身的候选窗问题。

**Disable SimpleIME.** Do not enable SimpleIME and Text Bridge together. When using Tailor, Modex or other UI mods, install their own required frameworks separately.

**请禁用 SimpleIME。** 不要同时启用 SimpleIME 与 Text Bridge。使用 Tailor、Modex 或其他界面模组时，请另行安装它们各自要求的框架。

## Your input. Your settings. / 你的输入，你的设置

**Candidate panel & input method.** The panel shows composition text, candidates supplied by the IME and the active input method's name. If Windows exposes only language information, it is labeled as the keyboard language. The selected input method and Text Bridge's Chinese/English mode are displayed separately.

**候选面板与输入法识别。** 面板显示输入法提供的拼音组合、候选词及当前输入法名称；Windows 只提供语言信息时，则明确标注为“键盘语言”。系统选中的输入法与 Text Bridge 的中英文模式分别展示。

**Hotkey filtering.** During Text Bridge input, ordinary keyboard presses, held events and releases are filtered from the downstream game input chain. Mouse and common editing keys remain available; Esc is reserved for cancellation and exit. Hotkeys bound to editing keys, earlier hooks or independent keyboard polling can still react. Prisma native input is not filtered by Text Bridge.

**热键过滤。** Text Bridge 输入期间，会过滤发往后续游戏输入链的普通键按下、持续和松开事件。保留鼠标与常用编辑键；Esc 用于取消组合及退出。绑定在编辑键上的热键、较早的输入钩子或独立轮询键盘的模组仍可能响应。Prisma 原生输入不应用本模组的过滤。

**In-game customization.** With SKSE Menu Framework, adjust the mod switch, toggle key, automatic activation, panel position, scale, colors and background opacity. Text stays readable with a transparent background. Save settings to retain them between sessions.

**游戏内自定义。** 安装 SKSE Menu Framework 后，可调整模组开关、快捷键、自动开启、面板位置、缩放、颜色及背景不透明度。背景透明时文字仍保持清晰。点击保存可在下次启动游戏时保留设置。

## Before you start / 前置要求

**Required: Windows, SKSE64 and Address Library matching your Skyrim runtime, plus [Media Keys Fix SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/92948).** Install a Chinese IME and use windowed or borderless mode.

**必需：Windows、与游戏运行时匹配的 SKSE64 和 Address Library，以及 [Media Keys Fix SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/92948)。** 请安装中文输入法，并使用窗口化或无边框模式。

**Media Keys Fix SKSE is required for the normal setup.** It provides the keyboard environment needed for Windows IME messages. Without it, the input panel may open while text, composition or candidates fail to work. This requirement does not mean that Prisma UI must be installed. If you use another patch providing the same nonexclusive keyboard behavior, that equivalent setup must be verified separately.

**常规安装需要 Media Keys Fix SKSE。** 它提供 Windows 输入法消息所需的键盘环境；缺少它时，可能出现面板能打开，但文字、拼音组合或候选词无法正常工作的情况。这一依赖不意味着必须安装 Prisma UI；如果自行使用提供相同非独占键盘行为的其他补丁，需要另行确认其效果。

**Optional: SKSE Menu Framework.** It supplies the settings page; basic F8 input does not require it. Automatic activation can also be configured using the user INI.

**可选：SKSE Menu Framework。** 它提供设置页面，基础 F8 输入无需安装；自动开启也可以通过用户 INI 配置。

The DLL accepts **Skyrim SE 1.5.97 and AE 1.6.1170**. User testing has confirmed SkyUI and Meridian/Tailor automatic activation and the improved Esc exit in the current 1.6.1170 setup. **1.5.97 still needs in-game verification.**

DLL 接受 **Skyrim SE 1.5.97 与 AE 1.6.1170**。用户已在当前 1.6.1170 环境确认 SkyUI、Meridian/Tailor 自动开启及此次 Esc 退出优化正常；**1.5.97 仍待游戏实测。**

## Get started / 安装与使用

Install the release ZIP with MO2 or your mod manager. It contains only the DLL, default INI and one readme.txt with usage and complete license notices. Exit Skyrim before updating, enable only one Text Bridge version and disable SimpleIME. Preserve your existing SkyrimTextBridge.user.ini.

通过 MO2 或其他模组管理器安装发布 ZIP。包内只有 DLL、默认 INI 和一个合并使用说明及完整许可声明的 readme.txt。更新前退出游戏，只启用一个 Text Bridge 版本，并禁用 SimpleIME；保留已有 SkyrimTextBridge.user.ini。

Click a supported field and press **F8** to enter input mode. Type and select candidates normally. **Ctrl + Space** switches between Chinese and English typing. Changing mode cancels uncommitted composition, so select your candidate first. F8 can also close input mode; a custom toggle key replaces F8 wherever it is mentioned here.

点击受支持文本框，按 **F8** 进入输入模式，正常输入并选词。**Ctrl + 空格**切换中英文；切换会取消未提交的拼音，请先选词。F8 也可关闭输入模式；如自定义了快捷键，本文所有 F8 操作均改用你的快捷键。

After completing input, press **Esc** to exit. If composition is still active, Esc requests cancellation first. If committed characters are still queued, that Esc stays in the input flow while they are delivered; press Esc again to exit. The exit press and release do not trigger the game's normal Esc action; a later press can close the UI or open the pause menu. If a movement key was held during typing, release it before pressing it again.

输入完成后按 **Esc** 退出；仍有未选定拼音时，Esc 会先请求取消组合。刚选定的文字还在提交队列中时，该次 Esc 会留在输入流程内等待送达，再按一次即可退出。退出的按下及松开不会触发游戏原来的 Esc 操作；之后再按 Esc 可正常关闭界面或打开暂停菜单。若输入期间移动键仍按住，请先松开再重新按下。

For automatic activation, open **SKSE Menu Framework (F1 by default) → Skyrim Text Bridge → 输入法与面板设置**, enable **文本框聚焦时自动开启** and save. The setting is off by default. Alternatively, set **AutoEnable=1** under **[Input]** in SkyrimTextBridge.user.ini.

自动开启设置路径为 **SKSE Menu Framework（默认 F1）→ Skyrim Text Bridge → 输入法与面板设置**，勾选 **文本框聚焦时自动开启** 并保存，默认关闭。也可在 SkyrimTextBridge.user.ini 的 **[Input]** 下设置 **AutoEnable=1**。

Automatic sessions end when focus leaves; changing fields clears the old session's uncommitted composition and pending text. Opening settings, changing menus, loading a save or losing game focus also ends input mode. Prisma interfaces continue to use their own native input.

自动会话失焦后结束，换框时清理旧会话未提交的拼音及待发送文字。打开设置、切换菜单、读档或游戏失焦也会结束输入模式。Prisma 界面继续使用自身原生输入。

## Things to know / 使用须知

Some IMEs do not expose candidates through IMM, so their candidate lists cannot be shown by this panel. Name detection depends on Windows information. Automatic activation covers only the fields described above; alternative keyboard layouts and dead-key combinations have not been fully tested.

部分输入法不会通过 IMM 提供候选列表，本面板无法展示此类候选词。名称识别取决于 Windows 提供的信息；自动开启仅覆盖前述字段，其他键盘布局与死键组合尚未充分测试。

Text Bridge keeps the selected Windows input method when returning game input to English on exit. It does not install an IME, provide translation, or alter a context already taken over by Prisma or another framework.

退出时让游戏输入恢复英文状态，但保留 Windows 已选中的输入法。本模组不安装输入法、不提供翻译，也不修改已由 Prisma 或其他框架接管的输入上下文。

## Open source & feedback / 开源与反馈

Original Text Bridge code is licensed under **GPL-3.0-only**. Third-party dependencies retain their licenses; full notices are included in readme.txt. Project: [GitHub](https://github.com/BlackMesa79/Skyrim-Text-Bridge). A matching source ZIP accompanies this release. When reporting issues, include the Skyrim runtime, IME name, related mod versions, reproduction steps and SkyrimTextBridge.log. Entered text is not logged.

Text Bridge 自身代码采用 **GPL-3.0-only**，第三方依赖保留原有许可，完整声明见 readme.txt。项目：[GitHub](https://github.com/BlackMesa79/Skyrim-Text-Bridge)，本版本提供对应源码 ZIP。反馈时请附游戏运行时、输入法名称、相关模组版本、复现步骤及 SkyrimTextBridge.log；日志不会记录输入文字。
