# RaceMenu automatic activation local test — 2026-10-07

Branch: `codex/racemenu-autofocus`, based on 0.3.11 release commit `5659940`. Version: 0.3.12-beta.1. No public release created.

## Cause and implementation

The production Scaleform auto-focus scanner admitted Inventory, Magic, Container and Barter menus only. RaceMenu uses engine menu name `RaceSex Menu`, which was excluded, explaining the missed automatic activation. Read-only inspection of the locally installed RaceMenu.bsa found RaceSex movie naming/text-entry controls, Selection usage, and SetAllowTextInput. No game UI assets or RaceMenu binaries are redistributed.

Add `RaceSex Menu` to the scanner, retaining both engine textEntryCount permission and Selection.getFocus (with _global fallback). Require the focused field type to be input and non-password. Also reject invisible/disabled fields and ancestors to prevent stale focus after a modal dialog is closed. Apply this guard to the existing SkyUI paths too. The optional auto-enable setting stays off by default and the existing manual-off suppression and focus-loss cleanup are retained. Other editable fields in the same menu may qualify; this is not a naming-dialog-only hardcoded path.

## Checks and limits

15 mock Scaleform tests execute the production scanner: text permission, naming activation, hidden-parent stale focus, reopening, passwords, readonly labels, disabled controls, global focus fallback, missing API, blur, SkyUI regression, unsupported menus, absent movie and absent singleton. Full build also runs 61 core, 43 direct IME and 78 runtime address/queue assertions plus existing DOM/API/settings/metadata checks.

Mocks do not prove the installed RaceMenu movie's actual runtime focus path, timing or output behavior. Naming must be tested in game with AutoEnable enabled. Four-runtime admission stays exactly 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104; prior 1.6.1170 feedback does not validate this new binary. Other runtime game-test status remains unchanged.

## Exclusive fullscreen feasibility

Current ImePanel uses a GDI-rendered WS_EX_LAYERED/NOACTIVATE/TRANSPARENT Windows popup shown topmost. True DXGI exclusive fullscreen can bypass desktop composition, so topmost does not guarantee candidate/status visibility. Windows fullscreen optimizations may mask this by promoting composition; that does not establish true-exclusive support. IME text delivery and candidate rendering are separate. Media Keys Fix nonexclusive keyboard cooperative access is also independent of display exclusivity.

Reliable support is feasible by drawing candidate/status UI inside the game render pipeline (for example a D3D11 overlay or an appropriate Menu Framework render callback), retaining the existing IME association and character delivery. That requires render-thread snapshots, state-preserving rendering, swap-chain/device transition handling, fonts and framework coexistence tests. Native TSF candidates that are not available through IMM may also require a TSF candidate interface. No renderer or full-screen mode change is implemented in this build; use windowed/borderless meanwhile.

## 游戏测试

1. 可选设置中开启“文本框聚焦时自动开启”并保存。进入 RaceMenu，仅操作滑块时不应开启。
2. 完成捏脸，打开角色名称输入框，确认无需 F8 即出现面板，中文候选与选定文字送入名称框；Ctrl+空格英文也正常。
3. 取消命名/返回捏脸，应结束自动会话；重新打开命名框可自动开启。手动 F8/完成后 Esc 退出时，同一焦点不应立刻重开。
4. 检查预设命名、加载等对话框中真正聚焦的编辑框；不可编辑、隐藏、禁用及密码框不应触发。
5. SkyUI 搜索、Meridian/Tailor 自动开启、Modex 手动和 Prisma 原生共存保持正常。退出输入后移动无拼音残留。

可回退到保留的 0.3.11 ZIP；同步会备份被覆盖的文件并保留个人 user.ini。请只启用一个 Text Bridge 安装。

Test DLL SHA256: `424CC921C2E4FF8B5B8AA01CCA6E38E1024745F38E59B17EEFE1CB283525DB22`.
