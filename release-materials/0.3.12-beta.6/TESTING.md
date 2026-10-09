# Native English and compact emoji panel — 2026-10-09

Local test build `0.3.12-beta.6`, based on `483a3d1`, branch `codex/imgui-candidate-panel`. No public push/release. User confirmed beta.5 SkyUI/RaceMenu automatic activation, Meridian and Modex Esc. This new DLL still needs game tests.

## English state

Previous translation ran only when Text Bridge's private English flag was enabled with Ctrl+Space. Native English Windows/IME mode could produce no text because Skyrim need not call TranslateMessage, while the plugin filters the game's keyboard/character events during typing. The bridge now samples the selected layout and existing window-thread TSF/IMM state before associating its private context, and mirrors live state on sync/key/IME/layout notifications. Plain non-CJK/non-IME layouts are English regardless of stale TSF conversion. No TSF manager activation, foreign profile selection or document focus changes are added.

Native English gets the same explicit key translation as the previous shortcut mode; Shift is honored, control/Alt editing shortcuts are excluded, Chinese composition is not translated. WM_CHAR copies are deduplicated against a bounded translated echo queue; unsolicited WM_CHAR Unicode, including surrogate pairs, remains available. Ctrl+Space still explicitly requests a mode change and cancels pending preedit. IME's own mode-switch key and system layout changes update the panel. Existing Esc/gameplay English-mode cleanup remains. Consequently, after exiting in English, the next session starts English unless the user changes it; automatic activation does not force Chinese on entry.

## Panel and supplementary glyphs

Candidate text: 17 -> 22 base units; row height/spacing: 34 -> 28. Header, composition and idle text are enlarged; overall header/idle height reduced. Position, opacity and scale settings remain.

ImGuiMCP exposes 16-bit ImWchar and a framework-selected font atlas; many emoji lie above U+FFFF. Preserve UTF-16/UTF-8 text and detect missing/unsupported glyphs without narrowing them to 16 bits. DirectWrite shapes the complete fallback label (including emoji sequences and Chinese) with system fallback fonts. Private offscreen D3D11 WARP + Direct2D draws color glyphs to CPU pixels; WIC encodes a random temporary PNG. Framework public LoadTexture/DisposeTexture uploads/caches it and ImDrawList_AddImage draws it within the same in-game panel. No game device/foreign ImGui atlas mutation, bundled fonts or desktop overlay. The image is removed immediately after loading; texture cache has at most 32 entries, at most two new rasters per HUD frame, and ages out invisible/old entries after a safe submitted-frame interval. Cache keys include text, size, dimensions and color; labels are not logged or used as filenames.

Only missing labels take this path. The framework's Chinese PrimaryFont remains required for ordinary text. Optional texture exports are checked; if missing, normal text/input remains available but this fallback cannot display emoji. Local SKSE Menu Framework 3.18 PE exports were audited read-only: LoadTexture, DisposeTexture, ImDrawList_AddImage and original HUD/font/draw exports present. The earlier local framework source public texture loader was reviewed; full game upload still needs testing.

System font coverage controls emoji appearance. The real preview shows colored 😂, a joined 👩‍💻 and ❤️. Windows regional indicators render as letters rather than a flag in the local font; no universal emoji claim is made. Supplementary text remains intact for submission, but the destination editor's own Unicode/font capabilities determine its final appearance.

## Validation

Complete local build passed: 66 core, 26 production Modex, 91 framework focus, 60 DirectIme lifecycle, 14 real isolated TSF read/write, 10 real Windows font/raster, 36 ImGui panel checks and 78 runtime queue/address checks. Meridian DOM/ABI, Prisma V1, IME naming, settings and DLL metadata also pass. DirectIme's IME mode in its harness is a deterministic double; its HWND/subclass/ToUnicode path is real. Real isolated TSF tests independently cover production mode reads/writes. Real font tests inspect colored nonempty emoji pixels, question-mark difference, joined/mixed text, bounds, UTF-8 rejection and PNG encoding. Panel fixtures exercise the real raster and a mock public texture API, reuse, file removal and retirement. No Skyrim entry point or Sogou runtime was executed.

Production HUD preview uses real ImGui 1.90.8 draw commands with MiSans and actual fallback PNG textures; inspected enlarged compact layout, mixed text, color emoji and row clipping. Viewport clamp/transparent background checks pass.

DLL SHA256: `80C0E9D4C4EBE16D0D00AC8D072982F9E4D9C74CF24161A1448C0D948D5C2063`.

Exact admitted game runtimes remain 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. These offline checks do not validate hooks or full game behavior in any runtime.

## 游戏回归

1. 英文（美式等）键盘布局下，F8 或自动聚焦进入后直接输入 a-z、数字、空格、Shift/CapsLock 大写和标点，无需 Ctrl+空格。不重复字符，按住键连续输入正常。
2. 微软拼音/搜狗自身英文状态下同样直接输入；使用输入法自身的 Shift 等切换键切回中文，正常拼音和选词、不泄露原始拼音。Win+空格切换布局/同语言配置，面板状态与名称刷新。
3. 中文选词和提交仍正常；Ctrl+空格作为可选切换保留。Esc 退出保持英文清理，随后 WASD 正常；重新聚焦如果仍为英文应直接输入英文，需要中文时正常切换。
4. 输入 haha 等有 Emoji 候选的词，候选面板常用 Emoji 显示为系统字形，字体/行距明显改善；缩放、透明度、不同分辨率和候选页刷新无截断/残留纹理。选中提交后目标编辑器是否显示图形另外检查。
5. SkyUI 四类搜索框与 RaceMenu 命名自动开启回归；Meridian/Tailor 中文/英文送达；Modex 3.0.2 手动 F8、退出输入 Esc 不关菜单、下一次普通 Esc 关闭后立即移动；Prisma/Outfit Wheeler 不接管原生输入。
6. Alt+Tab、取消输入、读档、切换菜单、独占全屏和渲染组合回归。问题附准确框架/输入法/游戏版本和 SkyrimTextBridge.log。

Installation ZIP remains exactly DLL, default INI and license-inclusive readme.txt. Source/symbols separate. Sync backs up replacements and preserves settings/MO2 metadata. Old packages are retained. No game process is terminated.
