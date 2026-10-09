# Console auto-activation exclusion — 2026-10-09

Local test build `0.3.12-beta.8` on `codex/imgui-candidate-panel`, based on beta.7 (`9d391ba`). No public push/release. The user confirmed beta.7 startup and normal input behavior, then requested console exclusion and cancelled emoji support.

## Changes

The auto-focus router checks the native console input context and the Console registry name on the active menu stack, under the engine menu lock. It does not execute ActionScript while holding that lock. An open console blocks both Scaleform and Meridian automatic targets, preventing stale fields underneath from taking over. Activation is checked again before starting. A console opened during a Scaleform field query invalidates that target. Closed cached console instances are ignored.

These checks affect only automatic detection. The manual F8 path remains available and does not call the automatic Begin guard. Existing automatically owned sessions end on opening the console; console manual sessions have no auto-owned identity, so an empty auto target does not end them.

Emoji rendering was removed at the user's request. Remove PanelTextRaster, its private WARP/D2D/DirectWrite renderer, PNG files, framework texture uploads and cache, and corresponding raster fixtures. The ImGui HUD uses the existing framework font/text exports. Normal Unicode input delivery is retained; unsupported glyphs may still appear as question marks. Do not claim emoji support. Chinese font configuration remains required.

## Validation

The production scanner/router fixture adds eleven checks for native/renamed console ownership, no stale ActionScript queries, delayed Scaleform/Meridian capture rejection, observer suppression, close/resume, open-during-query rejection, automatic session stop and manual-session retention. The full build passed: 66 core, 26 Modex guard, 102 framework focus, 60 DirectIme, 14 real isolated TSF, 32 panel and 78 runtime queue/address checks, plus name, Prisma, Meridian DOM/ABI, settings and DLL metadata. These checks do not start Skyrim. The DLL import audit confirms no DirectWrite/Direct2D/WIC dependencies; CommonLib's legacy D3D11 wrappers remain linked. Source and link inputs contain no private glyph renderer.

DLL SHA256: `2872B1362E5F58C6203526FDF7A297FD6A7B41C4530358538798EE13C642F385`.

## 游戏验证

1. 启用自动开启后打开控制台：不弹出候选面板，不开启本模组输入模式，英文控制台命令正常。
2. 控制台中按 F8：可以手动开启、输入中文，F8/Esc 退出后不会自动重开。
3. 从 SkyUI 搜索等自动会话打开控制台，确认旧输入会话退出；关闭控制台后重新聚焦正常自动开启。
4. SkyUI、RaceMenu 命名及 Tailor 自动开启、英文直接输入、中文选词和提交正常；Modex Esc 及 Prisma 原生输入回归。

The package contains exactly DLL, default INI and license-inclusive readme.txt; source/symbols separate. Sync backs up replacements and preserves personal user.ini/MO2 extras. Older archives remain available.
