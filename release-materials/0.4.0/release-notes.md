# Skyrim Text Bridge 0.4.0


Move the candidate panel into the game's ImGui HUD; enlarge text and tighten spacing, with configurable style and position.
候选面板改为游戏内 ImGui HUD 绘制，增大字号、缩小行距，支持调整样式与位置。

Detect editable Scaleform/Meridian fields for optional automatic activation, including SkyUI and RaceMenu naming. Keep the console manual.
按 Scaleform/Meridian 框架检测文本焦点，可选自动开启，覆盖 SkyUI 和 RaceMenu 命名；控制台保留手动开启。

Follow the IME's Chinese/English state for direct English typing. Improve Esc cancellation/exit and Modex input cleanup; preserve Prisma native input.
跟随输入法中英文状态，英文可直接输入；完善 Esc 取消/退出与 Modex 输入清理，避让 Prisma 原生输入。

Update to CommonLibSSE-NG 10.1.0. Admit 1.5.97, 1.6.1170, 1.7.99 and 1.7.104; all passed offline checks, with current behavior game-tested on 1.6.1170.
更新至 CommonLibSSE-NG 10.1.0，准入 1.5.97、1.6.1170、1.7.99 和 1.7.104；四版本离线检查通过，当前功能已在 1.6.1170 游戏中验证。

Required: matching SKSE64 and Address Library, Media Keys Fix SKSE (or a verified equivalent keyboard patch), and SKSE Menu Framework with HUD/ImGui exports and a configured Chinese font. Disable SimpleIME.
需要匹配的 SKSE64 与 Address Library、Media Keys Fix SKSE（或已验证等效键盘补丁）、提供 HUD/ImGui 导出且已配置中文字体的 SKSE Menu Framework；请禁用 SimpleIME。

Exact runtime admission: **1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104**. All four passed offline compatibility checks. Current behavior was user-tested on **1.6.1170**; **the other three still await full in-game validation**. Every prerequisite must independently support your runtime. Media Keys Fix 1.0.2's 1.7.104 behavior remains unconfirmed here.
精确准入 **1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104**，四版本离线检查通过；当前功能已在 **1.6.1170** 游戏测试，**其余三版本仍待完整游戏验证**。所有前置均需独立支持游戏运行时，Media Keys Fix 1.0.2 在 1.7.104 的表现尚未确认。

Input support: Meridian/Tailor, Modex, SkyUI and eligible Scaleform fields. Prisma/Outfit Wheeler compatibility means yielding to their native input. Emoji display is unsupported; true exclusive fullscreen still needs game validation.
输入支持：Meridian/Tailor、Modex、SkyUI 和符合条件的 Scaleform 文本框；Prisma/Outfit Wheeler 为避让原生输入的兼容共存。不支持 Emoji 显示，真正独占全屏仍需游戏验证。

Install **SkyrimTextBridge-0.4.0.zip** only. It contains DLL, default INI and a single license-inclusive readme.txt. The matching **source** archive and **symbols** archive are separate; do not install them as mods. Original code is **GPL-3.0-only**; third-party licenses are retained.
安装 **SkyrimTextBridge-0.4.0.zip** 即可，内含 DLL、默认 INI 及合并完整许可的单个 readme.txt；对应 **source** 源码包和 **symbols** 符号包另行提供，不作为模组安装。自身代码采用 **GPL-3.0-only**，保留第三方许可。

[Bilingual description / 双语详情页](https://github.com/BlackMesa79/Skyrim-Text-Bridge/blob/v0.4.0/release-materials/0.4.0/description-en-zh.md) · [Validation / 验证记录](https://github.com/BlackMesa79/Skyrim-Text-Bridge/blob/v0.4.0/VALIDATION-0.4.0.md)
