SKYRIM TEXT BRIDGE 0.3.10
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

INPUT SUPPORT / 输入支持
Meridian UI (including Tailor), Modex and SkyUI: F8 activates Text Bridge to enter text.
Meridian UI（如 Tailor）、Modex、SkyUI：按 F8 调用本模组输入文字。

PRISMA COMPATIBILITY / Prisma 兼容性
Prisma UI and Outfit Wheeler use their own native input. Text Bridge does not provide input support for them and steps aside to preserve their normal input.
Prisma UI 与 Outfit Wheeler 使用自身原生输入。本模组不为其提供输入支持，主动避让，不接管其正常输入。

SETUP / 安装
Install with MO2; disable SimpleIME. Requires Windows, matching SKSE64, Address Library and Media Keys Fix SKSE (or an equivalent nonexclusive keyboard patch). DLL runtimes: 1.5.97 / 1.6.1170; user-tested on 1.6.1170, 1.5.97 remains unverified in game. Use windowed or borderless mode.
通过 MO2 安装，禁用 SimpleIME。需要 Windows、对应版本的 SKSE64、Address Library 和 Media Keys Fix SKSE（或提供等效非独占键盘环境的补丁）。DLL 接受 1.5.97 / 1.6.1170；目前用户实测 1.6.1170，1.5.97 仍待游戏验证。使用窗口化或无边框模式。

Click a supported field → F8 → type/select candidates. Ctrl + Space switches Chinese/English; F8 exits. Switching language cancels uncommitted composition. Turn input mode off before changing fields or resuming gameplay.
点击受支持文本框 → F8 → 输入/选词。Ctrl + 空格切换中英文，F8 退出。切换语言会取消未提交的组合文本。更换文本框或恢复游戏操作前关闭输入模式。

Optional SKSE Menu Framework: F1 → Skyrim Text Bridge → 输入法与面板设置. Customize enabled state, toggle key, position, scale, colors and opacity. Save settings to persist them.
可选 SKSE Menu Framework：F1 → Skyrim Text Bridge → 输入法与面板设置，可调整开关、快捷键、位置、缩放、颜色及透明度。保存后下次启动保留。

EXITING INPUT / 退出输入
While Text Bridge is active, Esc cancels an ongoing composition first. Once input is complete, Esc exits Text Bridge and shows the off notification. If committed characters are still queued, that Esc stays local while the result is delivered; press Esc again to exit. The exit press and its release are consumed, so a later Esc can perform the UI's normal action.
Text Bridge 开启时，Esc 会先取消尚在选词的拼音；输入完成后，Esc 退出本模组并显示关闭提示。如果刚选定的文字尚在提交队列中，该次 Esc 会留在输入流程内等待文字送达，再按 Esc 即可退出。退出按下及对应松开会被拦截，之后再按 Esc 可执行界面原来的操作。

Normal exit, F8 off and menu reset cancel composition and close the restored game IME context so gameplay keys are no longer composed as pinyin. The selected keyboard layout is not changed; the next Text Bridge session reopens IME input. This cleanup yields to Prisma and does not close or replace a context already taken over by another framework.
普通退出、F8 关闭及菜单重置会取消组合并关闭恢复后的游戏 IME 上下文，让游戏按键不再作为拼音组合。不会切换所选键盘布局；下次进入 Text Bridge 会重新开启输入法。Prisma 接管或其他框架已更换上下文时，本模组避让，不关闭或替换它们的上下文。

If a movement key was held while typing, release it before pressing it again for gameplay. This prevents a held typing key from turning into unintended movement on exit.
如果退出时移动键仍按住，请先松开再重新按下，避免输入期间按住的键在退出时变成意外移动。

The user has confirmed the improved Esc exit in the current 1.6.1170 setup and approved release of 0.3.10. Automatic activation was also confirmed in SkyUI and Meridian/Tailor; 1.5.97 remains unverified in game.
用户已在当前 1.6.1170 环境确认 Esc 退出优化正常，并批准发布 0.3.10。SkyUI 与 Meridian/Tailor 的自动开启也已确认；1.5.97 仍待游戏实测。
AUTOMATIC ACTIVATION (OPTIONAL, OFF BY DEFAULT) / 自动开启（可选，默认关闭）
In the settings page, enable “文本框聚焦时自动开启” and save. Automatically starts in SkyUI searches in Inventory, Magic, Container and Barter menus, and supported Meridian/Tailor text fields. Modex stays manual. Prisma continues to use its native input and is never taken over.
在设置页勾选“文本框聚焦时自动开启”并保存。自动开启范围为 SkyUI 的物品、魔法、容器、交易菜单搜索框，以及受支持的 Meridian/Tailor 文本框。Modex 保持手动；Prisma 继续使用原生输入，本模组不接管。

F8 (or your configured toggle) still works. Turning it off suppresses automatic reopening for the current focus; leave the field and focus it again to rearm. Automatically started sessions close when focus leaves. Switching fields cancels uncommitted composition and queued characters from the old session. Readonly, disabled and password fields are excluded from automatic activation.
F8（或自定义快捷键）仍有效。手动关闭后，当前焦点不会自动重开；离开文本框再重新聚焦即可恢复。自动开启的会话在失焦时结束；切换文本框会清除旧会话尚未提交的拼音和待发送字符。自动开启会排除只读、禁用及密码框。

Automatic activation was user-tested in SkyUI and Tailor on 0.3.9; the user has also confirmed the 0.3.10 exit cleanup. The setting can also be configured with AutoEnable=1 under [Input] in SkyrimTextBridge.user.ini; missing keys default to off.
用户已确认 SkyUI 和 Tailor 自动开启正常，并确认 0.3.10 的退出清理有效。也可在 SkyrimTextBridge.user.ini 的 [Input] 下设置 AutoEnable=1；旧配置缺少此项时默认关闭。

OPTIONAL NATIVE CLIENT API / 可选原生客户端接口
Retains the 0.3.8 optional input-state/reset API for FavoriteWheel. Other mods are not required to use this API.
保留 0.3.8 的 FavoriteWheel 可选输入状态/重置接口，其他模组不必接入此接口。

LIMITATIONS / 限制
Hotkey filtering covers downstream ordinary keyboard events; editing keys, earlier hooks and independent polling can still trigger hotkeys. Mouse remains available. Allows Ctrl+A/C/X/V through outside composition; whether an edit operation works depends on the target UI. Some IMEs expose no IMM candidate list. Name fallback is labeled as keyboard language. Chinese UI only; Japanese/Korean localization is only a possible future direction.
热键过滤针对后续普通键盘事件；编辑键、较早钩子及独立轮询仍可能触发热键。鼠标保留；非选词期间允许 Ctrl+A/C/X/V 编辑组合键通过，实际编辑功能由目标界面决定。部分输入法不提供 IMM 候选列表；名称回退时标注为键盘语言。目前界面为中文，日语/韩语本地化仅为未来可能的方向。

LICENSE / 许可
Copyright (C) 2026 BlackMesa79. Original Skyrim Text Bridge code: GPL-3.0-only, without warranty. Complete GPL text and third-party notices follow in this same file. Third-party files are not relicensed.
Copyright (C) 2026 BlackMesa79。本项目自身代码采用 GPL-3.0-only，不提供担保。GPL 全文及第三方许可均合并在本文件下方；第三方文件不重新授权为本项目协议。

Corresponding source and build instructions are provided in SkyrimTextBridge-0.3.10-source.zip alongside this release. The source snapshot includes the matching SDK headers and dependency sources, with their original licenses. The project repository is listed below.
对应源码及构建说明随本版本一并提供于 SkyrimTextBridge-0.3.10-source.zip。源码快照包含匹配的 SDK 头文件与依赖源码，并保留其原有许可。项目仓库如下。
https://github.com/BlackMesa79/Skyrim-Text-Bridge

The optional SKSE Menu Framework runtime is separately installed, not bundled. Its API header is available in the source tree. No restriction is added against modification or reverse engineering for debugging modifications to an LGPL-covered component.
SKSE Menu Framework 运行时另行安装，不随包提供；其 API 头文件可在源码中取得。不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
