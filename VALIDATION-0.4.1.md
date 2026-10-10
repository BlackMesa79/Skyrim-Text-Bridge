# 0.4.1 release validation — 2026-10-10

Based on `911fc5f` (multilingual beta) and stable 0.4.0. Changes in this release: no-INI installation packages, complete legacy configuration migration, read-only initialization and explicit first-save creation, direct localized settings sidebar entry, version metadata and bilingual Nexus materials. No new game hooks or runtime whitelist changes.

Configuration checks exercise the actual Settings.cpp implementation: missing files use built-in defaults without creating files; complete legacy values migrate in memory; saving creates only the personal file and leaves legacy bytes unchanged; existing personal files are authoritative; missing personal keys use compiled defaults; Load does not change file bytes; unrelated keys survive Save; filesystem errors return save failure. Also covers persisted language/hotkey/auto activation/colors, invalid settings and panel opacity.

Build-Local.ps1 passes: 66 core, 102 framework/Scaleform focus, 26 Modex guard, 78 DirectIme, 20 real isolated TSF, 176 localization/CJK mode, 48 panel API and 90 four-runtime address/queue/layout checks, plus DOM, IME-name sanitization, Prisma negotiation, Meridian ABI and DLL metadata. Exact runtime admission remains 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. Game entry points are never executed by these checks.

The screenshot's extra English Settings hierarchy came from registering `Settings / <localized label>`: Menu Framework parses `/` as a path separator. Registration now passes only the localized label. Public API has no rename/remove function; restart the game to rebuild the sidebar. Page contents still update when language changes, while the sidebar translation updates on restart.

Beta visual checks use actual ImGui 1.90.8 vertices/font atlases: Japanese MiSans and Korean Malgun sample panels show complete glyphs and no clipped labels. Installed Menu Framework 3.18 provides all panel/settings exports (read-only PE inspection). Font files and framework configuration are not distributed or modified. The local framework currently enables only Chinese; Japanese/Korean testing needs corresponding font settings.

0.4.1 SVG cover retains the established dark/gold template with Chinese/Japanese/Korean examples, exported at 2560×1440 and 1280×720 and visually checked. Bilingual Nexus BBCode/Markdown/HTML are generated from paired English/Chinese paragraphs. Desktop (1280px) and mobile (390px) browser previews render with no horizontal overflow and were visually checked. Summary is 121 English characters, 59 Chinese characters, 182 combined with spacing.

Installation ZIP is enforced to contain exactly `SKSE/Plugins/SkyrimTextBridge.dll` and `readme.txt`, with the full project and third-party licenses. No INI, PDB or promotional assets. Symbols and matching source ZIP are separate. Deployment rejects INI files, refuses while Skyrim runs, backs up replaced files and hashes output; player files and old packages are retained.

Actual Japanese/Korean IME engines, clause/Hanja conversion, other admitted runtimes and exclusive fullscreen still need further in-game validation. Earlier stable Chinese/English input was user-tested on 1.6.1170. Offline fixtures are not evidence of universal IME or editor compatibility. The user authorized preparing this version for release; Nexus upload remains a separate external action unless explicitly requested. Source can be pushed to GitHub under the existing source-hosting authorization; no GitHub Release is created.

Built DLL SHA256: `7EB879231BF7F378DA44C8B6183CB63CA10FFF002EFCD7EC379A09B0A7F7C96D`.
