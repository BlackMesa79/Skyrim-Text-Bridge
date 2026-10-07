# Framework-based automatic activation — 2026-10-07

Local test version `0.3.12-beta.4` on `codex/imgui-candidate-panel`, based on `322b96b`. No public release or push. Stable 0.3.11 and older test packages remain available.

## Implementation and limits

`FrameworkFocus` selects framework ownership before probing fields: Prisma native input, then a focused Meridian view, then Scaleform. Prisma is coexistence only; it is never captured. Meridian retains its existing View/1 DOM adapter, covering eligible fields without identifying individual mods. Independent ImGui contexts such as Modex lack a universal public text-focus API and remain manual F8.

Scaleform no longer uses menu-name whitelists. It snapshots retained menu/movie references under the engine lock, releases that lock before fixed Selection focus queries, and checks the top input owner, positive game text-input permission, visible editable field and ancestors, and instance identity. Upper dialogs or unsupported custom renderers block stale underlying fields. It rechecks ownership after queries and before activation. Password, read-only, disabled and hidden fields are excluded. Detection does not prove end-to-end text delivery in every custom editor. Automatic activation remains optional and off by default; manual-off suppression and focus-loss cleanup remain.

See [framework design](../../FRAMEWORK-INPUT.md). Retains beta.3 IMM/TSF exit cleanup, Modex Esc/Cancel guard and scoped normal-close text-lease cleanup, plus the in-game ImGui candidate panel. These paths still need game regression testing.

## Modex 3.0.2 audit

Installed MO2 `Modex.dll` is byte-identical to the official asset in [v3.0.2](https://github.com/patchulidev/ModExplorerMenu/releases/tag/v3.0.2), commit `7035daece244aaddb2bbcca315d9d99f8df86af7`. DLL SHA256: `BDCA47F63131972A0767BF1F6BE44084DC4E85DCA04F6811E83822759086F5A4`; official release ZIP SHA256: `DA8CE1822A8B4FB78E59664475AA9D3E55A1A4BBA1612F3E8CAA09275A151746`.

The source bodies of `ModexGUIMenu::ProcessMessage`, `PostDisplay`, `UIManager::OnClose` and `TrySetAllowTextInput` are unchanged from v3.0.0 (`1b7673b1f0805c0fbe9acc26edd440df4d3cdf0a`). The existing public IMenu guard remains applicable at source level. VersionInfo still reports 3.0.0; the official asset hash identifies the installed binary. This audit is not an in-game reproduction or validation.

## Offline validation

Build completed successfully with the local Address Library for all four admitted runtimes. Checks passed:

- 66 core checks.
- 26 production Modex menu guard checks with engine data doubles.
- 55 production Scaleform/framework focus checks with mock engine/movies: unknown menu names, SkyUI/RaceMenu, fields/ancestors, input ownership/depth, passive overlays, closing/replacing menus during queries, lock release before queries, late activation, framework priority and manual-off/reentry.
- 52 DirectIme lifecycle checks with peer/panel doubles.
- 11 real isolated Windows TSF mode checks; explicit progress file ends in `COMPLETED 11`. No Sogou or game validation.
- 30 ImGui panel lifecycle/render checks with mock public API.
- 78 runtime queue/address checks for 1.5.97.0, 1.6.1170.0, 1.7.99.0 and 1.7.104.0; no Skyrim entry points executed.
- Meridian DOM/ABI, Prisma API negotiation/handoff, input-method naming, settings and DLL exports/version/runtime metadata checks.

Built DLL SHA256: `F66081B3CFEBB49D861D402E99538D34374D52170FC2C97C93A231FCBA26E26F`.

## 游戏验证

启用 F1 → Skyrim Text Bridge → 输入法与面板设置 → “文本框聚焦时自动开启”并保存。默认仍关闭，可以继续用 F8。

1. SkyUI 搜索框、RaceMenu 完成捏脸后的角色命名、Meridian/Tailor 文本框：聚焦后自动开启，中英文能送达，失焦/关闭后输入模式正常结束。仅打开菜单或捏脸界面不能误开启。
2. 其他 Scaleform 文本框：检查自动识别和实际文字送达；未提供焦点接口的字段保留 F8，不据此认定任意模组已经兼容。
3. 上层弹窗、自定义渲染菜单、隐藏/禁用/只读字段：不能激活底下残留焦点；快速关闭、重开或换框不沿用旧输入目标。
4. F8 手动关闭后保持同一焦点不重开，离开再聚焦后恢复自动开启；换框不提交旧框的未选定拼音。
5. Prisma/Outfit Wheeler：仍使用 Prisma 原生输入；有无开启自动设置/F8 均不能接管其输入，检查候选框和原生输入。
6. Modex 3.0.2：手动 F8 输入；选词中 Esc 先取消拼音，再 Esc 退出输入但不关闭菜单，下一次普通 Esc 正常关闭。检查正常关闭、反复打开、快速两次 Esc、长按及关闭后 WASD，避免残留输入锁。
7. 搜狗和微软拼音：提交后 Esc/F8 退出请求英文状态，再按 WASD 不残留拼音；重新进入中文及 Ctrl+空格切换正常。Alt+Tab、窗口化/无边框/独占全屏回归。

这份 DLL 未启动游戏验证；离线检查及旧版反馈不代表新版本在四个运行时已经实测。问题反馈附准确游戏/框架/输入法版本及 SkyrimTextBridge.log。

安装 ZIP 仅包含 DLL、默认 INI 和一个含完整许可的 readme.txt；源码、符号另包。同步只替换这三个文件，备份旧文件并保留个人 user.ini 与 MO2 元数据，不终止游戏进程。
