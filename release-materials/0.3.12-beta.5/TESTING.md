# SkyUI automatic-activation regression — 2026-10-09

Local test build `0.3.12-beta.5`, based on `ea46e10`, branch `codex/imgui-candidate-panel`. No public push or release. The user reported beta.4 SkyUI automatic activation failing, Meridian working and Modex Esc fixed. Those reports apply to beta.4, not this new DLL.

## Cause and change

Beta.4 treated CustomRendering as unsupported input and as keyboard ownership. Pinned CommonLib headers show Inventory/Magic/Container/Barter menus carry this flag and use depth 0. Inventory also has no menu context. Cursor (depth 13), HUD (depth 2) and Fader (depth 3) carry CustomRendering with no input context, masking the item menu in beta.4. The old fixture incorrectly represented SkyUI as a depth-3 ordinary menu and cursor as a noncustom kCursor context.

The scanner now probes the actual Scaleform movie regardless of CustomRendering. Inventory-item, pause and application flags also establish ownership. A layer with no active input role, no pause/inventory/application ownership, and persistent/cursor-assigned/saving-compatible custom drawing flags is passive. Modal/menu/active contexts override passive-looking flags. A top owner without a supported field/movie still blocks underlying stale focus. No mod-name whitelist, engine flag changes, shared input-counter reset or foreign ImGui access added.

Meridian/Prisma routing, Modex guard, IME lifecycle and input hook behavior remain. Automatic activation is still optional and off by default. Other custom editors need actual text-delivery validation.

## Validation

91 production scanner/router checks with mock engine/movie data, including 36 new assertions using native Inventory/Magic/Container/Barter flags/depths, cursor/HUD/fader overlays, late activation, blur, modal/pause/application blockers and ownership changes during queries. Also run the existing 66 core, 26 Modex, 52 DirectIme, 11 isolated real TSF, 30 ImGui and 78 runtime queue/address checks, plus Meridian DOM/ABI, Prisma API, input-method name, settings and DLL metadata checks. No Skyrim entry points or Sogou runtime are exercised by offline checks.

Exact admitted runtimes remain 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. Game validation pending for the new binary.

## 游戏回归

1. 保持 F1 设置页的“文本框聚焦时自动开启”勾选并保存。SkyUI 物品栏、魔法、容器、交易菜单：仅打开菜单不启用，点击搜索栏自动开启，中英文送达，失焦/关菜单后结束。
2. 手动 F8/Esc 关闭后，同一焦点不重开；离开再聚焦恢复。上层弹窗阻止底下残留焦点，鼠标光标/HUD 不应阻止搜索框。
3. RaceMenu 捏脸结束后的角色命名，以及新 Scaleform 文本框回归。
4. Meridian/Tailor 自动输入继续正常；Prisma/Outfit Wheeler 保留自身输入和候选框。
5. Modex 3.0.2 手动 F8：Esc 退出输入不关闭菜单，下一次普通 Esc 正常关闭，WASD 不被锁住。搜狗退出英文状态及独占全屏候选面板回归。

安装包仅 DLL、默认 INI、含完整许可的 readme.txt；源码和符号另包。同步前退出 Skyrim，保留用户设置/MO2 元数据，备份替换文件。反馈附 SkyrimTextBridge.log 与版本信息。

Build completed; all listed checks passed. DLL SHA256: AB9F3C315672EAD85A99BA7A797DA0A769FF3019F9D9D0A03B4D94CC9FD5BC79.
