# Pre-menu loader-lock deadlock fix — 2026-10-09

Local test build `0.3.12-beta.7`, based on `211e46f`, branch `codex/imgui-candidate-panel`. No public push/release. Beta.6 is affected by a startup deadlock and should not be used. Older archives are retained for correspondence/history; beta.5 is the prior user-validated fallback.

## Evidence from the hung game

Observed SkyrimSE PID 11500, runtime 1.6.1170, nonresponsive before the main menu. Latest Text Bridge log had only plugin loading lines, before kDataLoaded input-hook/panel registration. SKSE initialization completed. Read-only thread-context/stack-memory capture was taken locally, briefly pausing individual threads and always resuming them in finally blocks; no game process was terminated or settings modified. Raw evidence remains under ignored build/hang-thread-scan.txt. This was a symbolized stack-word scan, not a formal debugger unwind; nearest export symbols in external modules must not be interpreted as exact private functions.

Main thread 10424 was waiting in NtWaitForSingleObject with LdrLoadDll, Steam gameoverlayrenderer64, MO2 usvfs and hardware D3D11 device-creation addresses in its stack. Thread 33012 was waiting inside d3d11 device creation and had matching local beta.6 PDB symbols for TextBridge::PanelTextRaster::Renderer::Renderer, renderer's dynamic initializer, __dyn_tls_init and DLL/thread startup in its stack. This directly links beta.6's namespace thread_local renderer to loader-lock initialization while graphics creation waits. Multiple other threads waited on loader thread shutdown/initialization. There is no need to attribute the issue to unrelated mod updates.

The code used namespace-scope thread_local Renderer with COM/DirectWrite/Direct2D/D3D11 work in its constructor and destructor. MSVC executes dynamic TLS initialization on new thread attach while the loader lock is held. The offline executable raster tests had validated drawing, not real DLL/thread lifecycle, and missed this path.

## Fix

Remove dynamic TLS construction/teardown. A function-local pointer creates the private Renderer only after a valid UTF-8 fallback draw, with the engine/render callback running outside loader callbacks. One process-lifetime instance is confined to its original render thread; calls from another thread decline drawing. No renderer construction at plugin load, namespace initialization or thread attach. Windows reclaims the private graphics resources on process exit; no driver/COM teardown is registered for DLL detach. This intentionally supports the normal process-lifetime SKSE plugin, not hot unload. COM apartment references for actual render/PNG operations are scoped and balanced; no permanent COM activation is added to unrelated threads.

No changes to language-state following, frame layout, input/typing guards, Scaleform/Meridian routing, Prisma yielding or Modex lifecycle. Standard Emoji/system-font limitations remain as described in beta.6 testing notes.

## Validation

Build completed successfully. A new isolated real DLL lifecycle fixture covers normal LoadLibrary, zero renderer constructions before drawing, eight new-thread attach/exit cycles without construction, first actual glyph draw constructing once, later threads not creating devices, draw reuse and FreeLibrary completion: eight checks passed. Timeout is bounded to 30 seconds and only the isolated helper can be stopped by the test script.

Existing 66 core, 26 Modex guard, 91 framework focus, 60 DirectIme, 14 real isolated TSF, 10 real Windows font/raster, 36 public panel API fixture and 78 runtime queue/address checks passed. Meridian DOM/ABI, Prisma V1, input-method name, settings and DLL metadata passed. Exact admitted runtimes remain 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. Tests do not run Skyrim entry points or validate the new binary in-game.

## 游戏验证

1. 保持其他模组和配置不变，更新 beta.7 后重新从 MO2 启动，确认能进入主菜单。若仍卡住，保留最新日志以检查是否出现另一阻塞。
2. SkyUI/RaceMenu/Tailor 聚焦后正常自动开启，英文布局/输入法英文状态可直接打字，中文选词及提交正常。
3. 输入有 Emoji 候选的词，确认首次补绘不会卡死，候选面板正常显示；反复开启、关闭及 Alt+Tab 回归。
4. Modex 3.0.2 Esc 退出输入不关闭菜单，下一次普通 Esc 关闭后能移动；Prisma 原生输入避让及全屏回归。

Synced package contains exactly DLL, default INI and license-inclusive readme.txt; source/symbols separate. Replacements backed up, personal user.ini/MO2 metadata retained. User manually exited the hung game before deployment. No game process terminated by tools. This new game's startup still requires user confirmation.
DLL SHA256: 5F8FA913CA819387137EBD083E761001F4485C4508425F14848092B362DB6D01.
