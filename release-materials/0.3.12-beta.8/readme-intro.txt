SKYRIM TEXT BRIDGE 0.3.12-beta.8
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

STATUS / 状态
Local test update. The console is excluded from automatic activation; manual F8 remains available. Experimental emoji rendering is removed at the user's request. Retains direct English input, compact larger candidate text, framework focus detection and Modex Esc fixes. The user confirmed beta.7 startup and normal input; this new DLL requires game testing. Exact runtime admission: 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. Older packages remain available; avoid beta.6 due to its startup deadlock.
本地测试更新。控制台不再自动开启输入法，保留手动 F8；按用户要求移除实验性 Emoji 补绘。保留直接英文输入、更大更紧凑的候选文字、框架焦点检测和 Modex Esc 修复。用户已确认 beta.7 正常启动、输入正常，新 DLL 仍需游戏验证。精确准入版本：1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104；保留旧包，请避开存在启动死锁的 beta.6。

REQUIREMENTS / 前置
Matching SKSE64 and Address Library, Media Keys Fix SKSE or a working equivalent nonexclusive keyboard patch, and SKSE Menu Framework with HUD/ImGui exports. Configure EnableChinese=true and a Chinese-capable PrimaryFont under Data/SKSE/Plugins/fonts. Each prerequisite must independently support your runtime. 1.7.104 uses SKSE 2.3.1; Media Keys Fix 1.0.2 documents 1.7.99, and its 1.7.104 operation requires confirmation. Frameworks/fonts are not bundled.
需要匹配的 SKSE64 和 Address Library、Media Keys Fix SKSE 或可用等效非独占键盘补丁，以及提供 HUD/ImGui 导出的 SKSE Menu Framework。配置 EnableChinese=true 和 Data/SKSE/Plugins/fonts 中有效的中文 PrimaryFont。各前置需独立支持你的运行时。1.7.104 对应 SKSE 2.3.1；Media Keys Fix 1.0.2 说明列出 1.7.99，其在 1.7.104 的工作状态仍待确认。框架与字体不随包提供。

INSTALLATION / 安装
Exit Skyrim before updating through MO2. Disable SimpleIME and duplicate Text Bridge installations. Preserve SkyrimTextBridge.user.ini for personal settings. Installation ZIP contains only DLL, default INI and this readme.txt with complete license notices; corresponding source and symbols are separate. Retained older packages allow rollback without save editing.
退出游戏后通过 MO2 更新，禁用 SimpleIME 和重复安装。保留 SkyrimTextBridge.user.ini 以沿用个人设置。安装 ZIP 仅含 DLL、默认 INI 和本 readme.txt（含完整许可声明）；对应源码及符号另包。可回退保留的旧包，无需修改存档。

USE AND SETTINGS / 使用与设置
Focus a compatible field and press F8. Input follows the current keyboard/IME English or native state. English can be typed directly; use your IME's own switch key or optional Ctrl+Space to change mode. F8 exits. Esc first cancels live composition, then exits after committed text is delivered. Exit requests English state for gameplay; the selected IME profile is not changed. F1 -> Skyrim Text Bridge -> 输入法与面板设置 controls optional automatic activation (off by default), position, scale, colors, opacity, enabled state and hotkey. Manual off suppresses reactivation until focus leaves and returns.
聚焦兼容文本框后按 F8。进入时跟随当前键盘/输入法的英文或本地语言状态，英文可以直接输入；可用输入法自身的切换键或可选 Ctrl+空格改变状态。F8 关闭，Esc 先取消正在选词的拼音，再于文字送达后退出。退出为游戏操作请求英文状态，不更换所选输入法配置。F1 -> Skyrim Text Bridge -> 输入法与面板设置 可调整可选自动开启（默认关闭）、位置、缩放、颜色、透明度、模组开关及快捷键。手动关闭后，离开再聚焦才自动开启。

The console never automatically starts Text Bridge. It also blocks stale Scaleform/Meridian fields underneath from activating. To use IME in the console, press F8 manually. Close the console and focus a supported field to resume automatic activation.
控制台不会自动开启本模组，也不会误触发底下残留焦点的 Scaleform/Meridian 文本框。需要在控制台使用输入法时手动按 F8；关闭控制台并聚焦支持的文本框后恢复自动开启。

PANEL / 面板
The candidate panel is drawn inside the game through the framework's ImGui HUD/text API. Candidate font size is 22 and row spacing is 28 base panel units; position, size and colors are configurable. Uses framework fonts; emoji display is not supported. Missing glyphs may appear as question marks. No private rendering device, temporary images or fallback texture cache. Unicode text delivery is retained; the destination editor's fonts and Unicode handling determine its own display.
候选面板通过框架的 ImGui HUD/文本接口在游戏内绘制，候选字号 22、行距 28 个基础面板单位，可调整位置、大小与颜色。使用框架字体，不支持 Emoji 显示，缺失字形可能显示为问号。没有私有绘图设备、临时图片或补绘纹理缓存；Unicode 文字传递保留，目标编辑器的显示由其自身字体及 Unicode 处理决定。

COMPATIBILITY / 兼容性
Input support includes Meridian UI (such as Tailor), Modex, SkyUI and other compatible fields. Scaleform detects actual editable focus, including eligible RaceMenu naming fields; Meridian retains framework-wide DOM detection. The console is explicitly excluded from automatic activation. Password/read-only/disabled/hidden fields are excluded. Detection is not a blanket compatibility guarantee. Modex and independent ImGui interfaces remain manual F8. Prisma UI/Outfit Wheeler use native input; Text Bridge yields and does not supply IME input to Prisma interfaces. Retains Modex 3.0.2 Esc and close cleanup.
输入支持包括 Meridian UI（如 Tailor）、Modex、SkyUI 等兼容文本框。Scaleform 检测真实可编辑焦点，包含符合条件的 RaceMenu 命名；Meridian 保留框架级 DOM 检测。控制台明确排除自动开启，密码、只读、禁用、隐藏字段也排除。识别焦点不等于所有编辑器完整兼容。Modex 与独立 ImGui 界面继续手动 F8。Prisma UI/Outfit Wheeler 使用原生输入，本模组主动避让，不向 Prisma 界面提供输入法接入。保留 Modex 3.0.2 Esc 和关闭清理。

TESTING / 测试
Offline checks cover the console opt-out, focus ownership, language-state following, Unicode/deduplication, real isolated TSF state reads, public panel API fixtures and four-runtime metadata/address queues; no Skyrim entry point is executed. Test console normal English input, manual F8, console close/resume, SkyUI/RaceMenu/Tailor auto-activation, Modex Esc and Prisma native input. Exclusive fullscreen, other layouts/dead keys and newly detected editors need game validation. Keyboard filtering covers downstream events, not independent polling or earlier hooks; editing-key shortcuts may still trigger mods. Some IMEs provide no IMM candidate list. Chinese UI only; Japanese/Korean localization remains a possible future direction.
离线检查覆盖控制台排除、焦点归属、语言状态跟随、Unicode/重复过滤、真实隔离 TSF 状态读取、面板公开 API 替身及四运行时元数据/地址队列，未执行 Skyrim 入口。需实测控制台普通英文输入、手动 F8、关闭后恢复自动开启、SkyUI/RaceMenu/Tailor 自动开启、Modex Esc 及 Prisma 原生输入。独占全屏、其他布局/死键、新识别编辑器仍需游戏验证。键盘过滤覆盖后续事件，不覆盖独立轮询或较早钩子；编辑键热键仍可能触发模组。部分输入法没有 IMM 候选列表。界面目前为中文，日语/韩语本地化为未来可能方向。

LICENSE AND SOURCE / 许可与源码
Copyright (C) 2026 BlackMesa79. Original code: GPL-3.0-only, without warranty. Third-party files retain their licenses; complete notices and licenses follow. Corresponding source and build instructions are in SkyrimTextBridge-0.3.12-beta.8-source.zip. Provide corresponding source when redistributing binaries. SKSE Menu Framework is installed separately; its API header is supplied in source. No restriction is added against modifications or reverse engineering for debugging modifications to an LGPL component.
Copyright (C) 2026 BlackMesa79。自身代码采用 GPL-3.0-only，不提供担保；第三方文件保留原有许可，完整声明及许可合并于下方。对应源码及构建说明在 SkyrimTextBridge-0.3.12-beta.8-source.zip，转发二进制时须提供对应源码。SKSE Menu Framework 另行安装，源码包含 API 头文件；不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
