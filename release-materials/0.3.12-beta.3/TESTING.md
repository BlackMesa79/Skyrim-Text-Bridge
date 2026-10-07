# Esc / Sogou / Modex regression build — 2026-10-07

Version 0.3.12-beta.3 on `codex/imgui-candidate-panel`, based on 0020826. No public release or push. Keeps 0.3.11 stable and older test archives.

## Evidence and implementation

The older local Modex reference was obsolete. Installed MO2 metadata reports Modex 3.0.0; upstream tag v3.0.0 resolves to 1b7673b1f0805c0fbe9acc26edd440df4d3cdf0a. Reviewed upstream `src/core/Hooks.cpp`, `src/core/InputManager.cpp`, `src/ui/core/UIMenuImpl.cpp` and `src/ui/core/UIManager.cpp` through GitHub raw source. Its IMenu ProcessMessage handles the user event `Cancel` separately from Scaleform keyboard events. OnShow disables gameplay controls and OnHide restores them. TrySetAllowTextInput increments/decrements ControlMap textEntryCount during PostDisplay when WantTextInput changes, while OnClose clears ImGui keys without releasing that text lease. These are plausible causes matching the reported symptoms, not an in-game reproduction.

DirectIme now clears native/fullwidth/symbol conversion flags and closes IMM contexts, then writes thread-local TSF conversion/open compartments after owned-context destruction. Reopening enables native conversion. It queries the existing window-thread manager through TF_GetThreadMgr and uses ITfClientId for writes; no manager activation/deactivation, profile/layout switching or global compartment writes. It skips TSF writes on background focus loss, Prisma handoff and foreign-context takeover. Ctrl+Space uses the same mode setter. Windows Esc key/char messages stay local while attached; game input dispatch owns composition cancel/exit.

Modex guard validates the public IMenu ProcessMessage (slot 4) and PostDisplay (slot 6) implementations and vtable belong to the loaded Modex.dll before patching. No fixed module offsets or private Modex fields. Existing module-owned handlers are chained; absent/older/unrecognized menus decline installation. Thread-safe publication prevents simultaneous refresh installs. The guard consumes Cancel and Scaleform Esc only during typing or its bounded release tail. The tail is armed before IMM teardown, expires without another key and clears immediately on the next ordinary Esc press. Other user messages and all hide/force-hide messages go through the original handler.

A normal kHide releases exactly one text-input lease only when the guard observed a matching +1 in Modex's synchronous callback and the current count remains consistent. A -1 made by Modex itself cancels repair; ambiguous/external count changes decline repair. Reopening restores a repaired lease so Modex's retained WantTextInput flag can release it normally in the next render. No resetting the shared count, gameplay-control flags, keyboard devices or all menus. This intentionally does not force-repair unknown third-party control locks or Modex's unhandled force-hide behavior. If the Modex menu disappears, the input hook also ends its manual Text Bridge session even when a close notification was missed.

## Offline validation

66 core checks (including time-bounded Esc release behavior), 52 direct IME lifecycle checks (Windows Esc, next ordinary Esc, TSF requested modes, foreign/Prisma/background safeguards), 26 production Modex message-handler checks using engine data doubles (Cancel, Scaleform, pending exit, second Esc, normal/forced hide forwarding, text-lease close/reopen, clean close, external ownership changes and no underflow), 11 real Windows TSF compartment checks in an isolated process. Existing 15 Scaleform focus, 30 ImGui rendering, 78 runtime queue/address checks, Meridian DOM/ABI, Prisma API, input-method name, settings and four-runtime metadata checks also pass.

The real TSF test activates/deactivates only its own isolated test manager; the plugin never does this. No Sogou runtime or Skyrim entry point was executed by these checks. Production vtable installation and complete game flows still require user testing.

## 游戏验证

1. 搜狗拼音：Modex、SkyUI、Tailor 中按 F8，完成并提交中文后按 Esc 退出；检查搜狗为英文状态，随后 WASD、其他键不会残留拼音。再次 F8 能重新输入中文。Ctrl+Space 中英文切换也正常。
2. 尚在选词时第一次 Esc 只取消拼音并保持输入面板；再按一次 Esc 退出输入。输入刚提交时允许队列送达，不丢字。
3. Modex：退出输入的 Esc 不关闭 Modex；松开后第二次普通 Esc 正常关闭菜单，立即能移动。快速连续按两次及长按 Esc 也测试。
4. Modex 搜索框仍聚焦时通过正常菜单关闭操作退出，验证无需重新打开/关闭就能移动。反复打开、聚焦、取消、关闭；未开启 F8 时也测试正常关闭。嵌套弹窗与通过其他模组打开的 Modex 选择器另行回归。
5. 微软拼音、英文输入、SkyUI/Tailor、RaceMenu 自动开启、Prisma/Outfit Wheeler 原生输入、Alt+Tab、独占全屏回归。

出现问题时附 SkyrimTextBridge.log 与准确 Modex/搜狗/游戏版本，日志应有 `Modex IMenu Esc/Cancel guard and scoped text-lease cleanup installed`。切勿把四版本白名单、离线测试或旧版反馈写成新 DLL 的实测结论。

Installation archive still contains exactly DLL, default INI and readme.txt with complete license notices. Source and symbols separate. Sync preserves user.ini/MO2 metadata and backs up replaced files. No Skyrim process is terminated.

Built DLL SHA256: `A846E38AB377837B4D92C5EA9EBE448B026B1EA068667ECB70FBD167652B0A45`.
