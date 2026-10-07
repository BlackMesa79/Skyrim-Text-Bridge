SKYRIM TEXT BRIDGE 0.3.12-beta.4
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

STATUS / 状态
Local test build of framework-based automatic activation. Stable 0.3.11 and earlier test archives are retained. Exact runtime admission: 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. This new DLL and newly discovered fields require game testing. Old feedback and offline checks do not validate this binary on any runtime.
按 UI 框架自动开启的本地测试版，保留 0.3.11 正式包和旧测试包。精确准入版本：1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104。新 DLL 及新识别字段仍需游戏测试；旧反馈及离线检查不代表本 DLL 已在任何运行时实测。

CHANGES / 更新
Optional automatic input now routes by framework ownership. Scaleform detects actual focused editable fields in registered open input menus without a menu-name whitelist; includes eligible SkyUI, RaceMenu and custom Scaleform interfaces. Validate top input owner, movie/field visibility, text permission and instance identity; upper dialogs and custom renderers prevent activation of stale fields underneath. Re-check ownership after focus queries. Meridian keeps framework-wide DOM field detection. Prisma retains native input.
可选自动输入改为按框架归属选择后端。Scaleform 不再使用菜单名称白名单，统一识别已注册打开输入菜单中的真实可编辑焦点，包含符合条件的 SkyUI、RaceMenu 和其他 Scaleform 界面。核对最上层输入归属、影片/字段可见性、输入许可及实例身份；上层弹窗和自定义渲染器阻止底下的残留字段误开启。查询焦点后再次核对归属。Meridian 沿用框架级 DOM 检测，Prisma 保留原生输入。

Keep beta.3's explicit IMM/TSF English-mode cleanup, Modex Esc/Cancel interception and scoped text-input cleanup on normal close. Installed Modex.dll was verified identical to the official 3.0.2 release; upstream ProcessMessage, PostDisplay, OnClose and TrySetAllowTextInput are unchanged from 3.0.0. This source/binary audit does not replace game testing. Independent ImGui interfaces including Modex remain manual F8 because no universal public field-focus API exists.
保留 beta.3 的 IMM/TSF 英文状态清理、Modex Esc/Cancel 拦截和正常关闭时的局部文本输入清理。本地 Modex.dll 与官方 3.0.2 发布包完全一致，上游 ProcessMessage、PostDisplay、OnClose 和 TrySetAllowTextInput 与 3.0.0 相同；源码/二进制检查不代替游戏实测。Modex 等独立 ImGui 界面缺少统一公开文本焦点接口，继续手动 F8。

REQUIREMENTS / 前置
Windows, matching SKSE64 and Address Library, Media Keys Fix SKSE or a working equivalent nonexclusive keyboard patch, and SKSE Menu Framework with HUD/ImGui exports. The panel uses game render output; it requires EnableChinese=true and a Chinese-capable PrimaryFont under Data/SKSE/Plugins/fonts. Local framework 3.18 exports and existing MiSans configuration were checked. Fonts/framework DLLs are not bundled. Each prerequisite must support your game runtime independently. SKSE 2.3.1 is required for 1.7.104; Media Keys Fix 1.0.2 documents 1.7.99, while 1.7.104 operation still requires confirmation.
需要 Windows、匹配的 SKSE64 和 Address Library、Media Keys Fix SKSE 或可用等效非独占键盘补丁，以及提供 HUD/ImGui 导出的 SKSE Menu Framework。面板绘制到游戏画面，需 EnableChinese=true 及 Data/SKSE/Plugins/fonts 中有效的中文 PrimaryFont；本地已检查框架 3.18 导出和既有 MiSans 配置。字体及框架 DLL 不随包提供。各前置需独立支持你的游戏版本；1.7.104 对应 SKSE 2.3.1，Media Keys Fix 1.0.2 说明列出 1.7.99，其在 1.7.104 工作状态仍待确认。

INSTALLATION / 安装
Exit Skyrim before updating through MO2. Disable SimpleIME and duplicate Text Bridge installations. Keep SkyrimTextBridge.user.ini for personal settings. This installation ZIP contains only DLL, default INI and this readme.txt with full license notices. Source and symbols are separate archives. No save edit is needed to roll back to a retained earlier package.
退出游戏后通过 MO2 更新，禁用 SimpleIME 和重复 Text Bridge 安装。保留 SkyrimTextBridge.user.ini 以沿用个人设置。安装包仅含 DLL、默认 INI 和本 readme.txt（含完整许可声明），源码及符号独立打包。可回退保留的旧包，无需修改存档。

USE AND SETTINGS / 使用与设置
Focus a compatible field and press F8; Ctrl+Space switches Chinese/English. F8 exits. Esc first cancels live composition, then exits after committed characters are delivered. Exiting requests English state without changing the selected input-method profile. For automatic activation, enable the optional setting in F1 -> Skyrim Text Bridge -> 输入法与面板设置 and save; it remains off by default. Manual off is respected until focus leaves and returns. Position, size, colors, background opacity, enabled state and hotkey are configurable.
聚焦兼容文本框后按 F8，Ctrl+空格切换中英文，F8 关闭。Esc 先取消选词中的拼音，待文字送达后退出；退出时请求英文状态，不切换所选输入法配置。自动开启在 F1 -> Skyrim Text Bridge -> 输入法与面板设置 中勾选并保存，默认关闭。手动关闭后，离开文本框再重新聚焦才自动开启。可配置位置、大小、配色、背景透明度、模组开关和快捷键。

COMPATIBILITY / 兼容性
Input support remains Meridian UI (such as Tailor), Modex, SkyUI and other compatible fields. Generic Scaleform detection expands automatic discovery, not a blanket claim that every mod's text delivery is tested. Password, read-only, disabled and hidden fields are excluded; editors without reliable focus capability remain manual. Modex uses F8. Prisma UI and Outfit Wheeler use their own native input; Text Bridge yields and does not provide IME input for Prisma-based interfaces.
输入支持仍包括 Meridian UI（如 Tailor）、Modex、SkyUI 等兼容文本框。通用 Scaleform 检测扩展自动识别范围，不代表所有模组文字提交都已实测。排除密码、只读、禁用及隐藏字段，缺少可靠焦点能力的编辑器保留手动。Modex 使用 F8；Prisma UI 与 Outfit Wheeler 使用自身原生输入，Text Bridge 主动避让，不为 Prisma 界面提供输入法支持。

TESTING AND LIMITATIONS / 测试与限制
Test automatic activation in SkyUI, RaceMenu naming, Tailor and additional Scaleform editors; merely opening a menu must not activate typing. Verify blur, dialog overlays, manual-off/reentry, Chinese/English text delivery and Esc. Repeat Modex 3.0.2 manual F8, two-step Esc and movement after close; test Sogou and Microsoft Pinyin plus Prisma coexistence. The in-game ImGui panel targets windowed, borderless and exclusive fullscreen, with fullscreen/render combinations still needing game tests. Keyboard filtering covers downstream events; earlier hooks, independent polling and editing-key shortcuts may still trigger mods. Some IMEs do not expose IMM candidate lists. Chinese UI only for now. Japanese/Korean localization is a possible future direction.
测试 SkyUI、RaceMenu 命名、Tailor 和其他 Scaleform 编辑界面的自动开启，仅打开菜单不可误开启；检查失焦、上层弹窗、手动关闭/重新聚焦、中英文送达及 Esc。回归 Modex 3.0.2 手动 F8、两步 Esc 和关闭后移动，测试搜狗、微软拼音及 Prisma 共存。游戏内 ImGui 面板目标涵盖窗口化、无边框和独占全屏，具体全屏/渲染组合仍需游戏测试。键盘过滤覆盖后续事件，较早钩子、独立轮询或绑定编辑键的快捷键仍可能触发；部分输入法不提供 IMM 候选列表。目前界面为中文，日语/韩语本地化为未来可能方向。

LICENSE AND SOURCE / 许可与源码
Copyright (C) 2026 BlackMesa79. Original code: GPL-3.0-only, without warranty. Third-party files retain their licenses; complete notices and licenses follow in this file. Corresponding source and build instructions are supplied in SkyrimTextBridge-0.3.12-beta.4-source.zip. When redistributing the binary, provide its corresponding source. SKSE Menu Framework is installed separately; its API header is included in source. No restriction is added against modification or reverse engineering for debugging modifications to an LGPL-covered component.
Copyright (C) 2026 BlackMesa79。自身代码采用 GPL-3.0-only，不提供担保；第三方文件保留原有许可，完整声明及许可合并于下方。对应源码及构建说明提供于 SkyrimTextBridge-0.3.12-beta.4-source.zip，转发二进制时需提供对应源码。SKSE Menu Framework 另行安装，源码包含其 API 头文件；不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
