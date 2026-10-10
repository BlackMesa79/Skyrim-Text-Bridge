# 0.4.1-beta.1 local test guide / 本地测试说明

This is a local development build on `codex/multilingual-input`, based on stable `9a82e65`. It is not a public release. The installation ZIP contains exactly DLL, default INI and readme.txt with full notices. Existing personal settings and stable packages are retained.
这是从稳定版 `9a82e65` 开发的 `codex/multilingual-input` 本地测试版，尚未公开发布。安装 ZIP 仅含 DLL、默认 INI 与合并完整许可的 readme.txt，保留个人设置及旧稳定包。

Language follows Windows display language automatically (not the current keyboard or IME): EN / zh-CN / ja / ko, otherwise English. Optional `[Interface] Language=1..4` overrides it; `0` or an absent key uses automatic. Settings explanations are reduced to three wrapped hints. Page translation updates immediately; sidebar translation requires restart.
默认按 Windows 显示语言自动选择英/简中/日/韩，其他回退英文，不根据键盘或输入法切换。可选 `[Interface] Language=1..4` 覆盖，`0` 或缺省即自动。设置说明缩为三段自动换行提示。页面翻译即时更新，侧栏名称需重启。

Framework fonts must support the chosen scripts. Enable the needed `[Fonts] EnableChinese=true`, `EnableJapanese=true`, `EnableKorean=true` in SKSEMenuFramework.ini and use a PrimaryFont containing the corresponding glyphs. Enabling ranges alone cannot supply absent glyphs. Restart Skyrim after font changes. Text Bridge does not change third-party settings or bundle fonts. A missing-script settings page falls back to English while retaining the preference.
框架字体必须支持所选文字。在 SKSEMenuFramework.ini 的 `[Fonts]` 启用需要的 `EnableChinese=true`、`EnableJapanese=true`、`EnableKorean=true`，并设置包含这些字形的 PrimaryFont，仅启用范围无法补全缺字。修改字体后重启。本模组不改第三方配置、不打包字体；设置页缺字时暂用英文并保留语言偏好。

Test the following with actual IMEs in SkyUI and Meridian/Tailor, plus manual F8 in Modex. Translation correctness and real candidate behavior should be verified by Japanese/Korean users.
请在 SkyUI、Meridian/Tailor 使用真实输入法测试，并验证 Modex 手动 F8；翻译准确性和实际候选行为也需要日/韩用户确认。

1. Windows language automatic selection; manual override/save/restart; older user INI settings preserved.
   Windows 语言自动识别，手动覆盖/保存/重启，旧个人 INI 设置保留。
2. Japanese: Latin → Hiragana/Katakana, kana/fullwidth preservation, candidates, Enter commit, partial clause commit, Esc cancel/exit, punctuation and native IME language keys.
   日语：英文切平/片假名，假名/全角保留，候选、Enter 提交、分节提交、Esc 取消/退出、标点及输入法自身切换键。
3. Korean: Hangul/Latin key, incremental syllable preedit, no leaked Latin keys, final syllable commit, backspace, Hanja candidates if supplied, Esc cancel/exit.
   韩语：韩/英切换键，逐字组合，不泄漏英文字母，末尾音节提交、退格、有候选时的 Hanja 转换、Esc 取消/退出。
4. Chinese/English regression: direct English, Chinese candidates, F8 and Ctrl+Space, gameplay WASD after exit, auto focus in SkyUI/RaceMenu/Tailor, console opt-out, Modex Esc cleanup, Prisma native input.
   中/英回归：直接英文、中文候选、F8 和 Ctrl+空格、退出后 WASD、SkyUI/RaceMenu/Tailor 自动聚焦、控制台排除、Modex Esc 清理、Prisma 原生输入。

Offline validation: 66 core, 102 focus/router, 26 Modex guard, 78 DirectIme, 20 isolated real TSF, 176 localization/CJK policy, 48 public panel API fixture and 90 four-runtime address/queue/layout checks, plus DOM, settings migration/persistence, IME name, Prisma, Meridian ABI and DLL metadata. Composition payloads/candidates use fixtures; no actual Japanese/Korean engine or Skyrim entry point is exercised. Exact whitelist remains 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104. Every prerequisite needs matching runtime support; Media Keys Fix SKSE remains required for the normal setup.
离线验证：核心 66、焦点/路由 102、Modex 防护 26、DirectIme 78、真实隔离 TSF 20、本地化/日韩策略 176、公开面板 API 替身 48、四运行时地址/队列/布局 90，另有 DOM、设置迁移/持久化、输入法名称、Prisma、Meridian ABI 和 DLL 元数据检查。组合文字/候选使用替身数据，未运行实际日/韩输入法或 Skyrim 入口。精确白名单仍为 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104，各前置需匹配游戏，常规配置仍需要 Media Keys Fix SKSE。

Actual ImGui 1.90.8 preview renders Japanese (MiSans) and Korean (Malgun) without missing sample glyphs or clipped labels; development font files are not bundled. Read-only PE inspection confirmed every panel/settings export used in the installed Menu Framework 3.18. The local framework currently enables Chinese only; Japanese/Korean testing needs font configuration first.
实际 ImGui 1.90.8 预览中日语（MiSans）和韩语（Malgun）均无样本缺字或标签截断，开发预览字体不打包。只读 PE 检查确认本地 Menu Framework 3.18 提供全部所用面板/设置导出。本地框架目前只启用中文，日/韩测试前需配置字体。

Built DLL SHA256: `7276CEC3CB3C21F69EF7FDD5B9472B85BDEFC8E46EB4C0D4C8CA7F2156B6C066`.
构建 DLL SHA256 如上，用于确认安装包和本地部署一致。
