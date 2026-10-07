# In-game ImGui panel test — 2026-10-07

Branch: `codex/imgui-candidate-panel`, retaining the previous RaceMenu focus fix. Version: 0.3.12-beta.2. Stable 0.3.11 and earlier test archives are retained. No public release created.

## Implementation

Replace ImePanel's GDI surfaces and layered desktop popup with ImGui foreground draw-list commands in SKSE Menu Framework's `RegisterHudElement` callback. This callback runs after ImGui NewFrame on the framework's rendering thread every frame, even when no framework windows are open. Candidates, composition, input-method name and exit tips are published as locked UTF-8 snapshots from IME/window callbacks; drawing never accesses an IMM context.

The panel creates no ImGui window, takes no keyboard/mouse focus, sets no WantCapture flags and does not request a pause or cursor. It uses the framework-owned current font, viewport, draw list and device. No second ImGui context/atlas, separate Present hook or Direct3D resources are created. Font/device/viewport transitions belong to the framework; the panel reads current pointers every frame. Display scaling follows the actual render viewport, fits small viewports and respects nonzero viewport origins.

Background opacity is applied to ImGui RGBA colors independently from opaque text and accent. Preserve candidate selection, position, size, colors, literal text (including ##), UTF-8 clipping with ellipsis, and a 1.4-second transient off tip. Foreground loss/minimize removes stale snapshots. Rendering cannot activate text input on its own.

## Requirements and failure behavior

SKSE Menu Framework is now a required dependency for the panel. Check HUD, context, viewport, font/glyph, rect, text and measurement exports before registering anything. Missing/incomplete API or failed registration prevents the input dispatch hook from being installed and logs a cause. This avoids accepting filtered typing without a visible renderer. Existing settings page can remain available if its own exports are present.

Read-only PE inspection of the enabled local 3.18 DLL confirmed all ten required exports without executing its entry point. Source inspection confirmed HUD rendering while F1 is closed and the C ABI of ImFont_CalcTextSizeA (output pointer first). No framework structure fields except stable viewport geometry are read; font and draw-list pointers are opaque.

Chinese rendering requires a Chinese-capable PrimaryFont in SKSEMenuFramework.ini, EnableChinese=true, and the font file under Data/SKSE/Plugins/fonts. Current user setup enables 3.18 and MiSans-Regular.ttf from the framework CHS mod, with Chinese enabled. A missing Chinese glyph produces a log warning. No font is redistributed, injected into the shared atlas or configured by Text Bridge. A framework build compatible with the user's exact Skyrim version is required independently of Text Bridge's four-runtime whitelist.

Media Keys Fix SKSE/nonexclusive keyboard prerequisite remains; it does not configure display exclusivity. IMEs without an IMM candidate list are not fixed by rendering changes. Prisma continues to own its original input/candidate path.

## Validation

30 production-renderer checks with mock C exports cover complete/incomplete API, zero-ID success, registration failure, idle HUD, literal Unicode/## text, opacity endpoints, small/zero viewport, nonzero origin, UTF-8 truncation, immediate hide, tip timeout, focus loss, unavailable ImGui context, disabled mod and concurrent snapshot publication. Existing checks: 61 core, 15 RaceMenu/SkyUI focus, 43 direct IME, 78 runtime queue/address assertions, plus Meridian DOM/ABI, Prisma V1, input-method name, settings and metadata.

Visual QA additionally invokes the production draw function through actual ImGui 1.90.8 using the installed MiSans font. Its real draw vertices/indices/font texture were rasterized for inspection of Chinese labels, candidate highlight and translucent background. This is a standalone preview, not an in-game or exclusive-fullscreen validation.

## 游戏测试

1. F1 关闭时在 SkyUI、Modex、Tailor 按 F8：候选面板、当前输入法及中英文输入正常，鼠标点击目标框不被面板截获。
2. 设置页修改位置、缩放、颜色、0/50/100% 背景透明度，文字保持清晰；面板留在实际游戏视口内。
3. 窗口化、无边框及真正独占全屏分别测试。确认真正独占状态没有被 Display Tweaks/Windows 全屏优化改为无边框，检查候选可见与提交结果。
4. Alt+Tab、最小化、调整分辨率或切换显示模式后不残留旧面板；重新开启输入可以继续显示。退出后 1.4 秒提示消失。
5. RaceMenu 自动开启：开启设置后命名框激活，滑块操作/关闭对话框不误开启；取消和重新打开符合预期。
6. Prisma/Outfit Wheeler 原生输入共存、Esc 清理、WASD 移动、读档行为保持正常。不同渲染模组组合需分别确认。

真实全屏绘制能力取决于框架在该组合下的 D3D11 渲染链，新版仍需游戏验证。运行时范围继续精确为 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104，旧版用户反馈不等于本 DLL 或其他版本已实测。

安装 ZIP 仍只有 DLL、默认 INI、readme.txt；源码和符号另包。退出游戏后同步到原目录并备份被覆盖文件，保留 user.ini 和 MO2 元数据；仅启用一个 Text Bridge。可安装保留的 0.3.11 包回退到原 Windows 面板。

Built DLL SHA256: `81D03D33FE2A7E267E348268D8C1BE82C5DBD4D5AAFD667EA42F06075A663E49`.
