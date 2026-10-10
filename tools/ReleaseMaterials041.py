"""Generate the bilingual Nexus materials; cover stays editable SVG.
Run from any directory with Python 3. No third-party modules required.
"""
from pathlib import Path
import html
import re

root = Path(__file__).resolve().parents[1]
out = root / 'release-materials' / '0.4.1'
out.mkdir(parents=True, exist_ok=True)

title = 'Skyrim Text Bridge - Chinese Japanese and Korean IME Support'
summary_en = 'IME input for SkyUI, Modex and Meridian, with in-game candidates and optional auto activation. Compatible with Prisma UI.'
summary_zh = '为 SkyUI、Modex 和 Meridian 提供输入法、游戏内候选面板及可选自动开启，兼容 Prisma UI。'
assert len(summary_en) <= 160 and len(summary_en + summary_zh) + 2 <= 250
(out / 'title-and-subtitle.txt').write_text(title + '\n\nChinese Japanese and Korean IME Support\n中文、日语与韩语输入法支持\n', encoding='utf-8-sig')
(out / 'summary-en-zh.txt').write_text(summary_en + '\n\n' + summary_zh + '\n', encoding='utf-8-sig')

sections = [
('Type naturally. Stay in Skyrim. / 在天际，自然输入', [
('Type into supported Skyrim fields with [b]F8[/b], an in-game candidate panel, direct English typing and keyboard hotkey filtering. Optional automatic activation starts input when a supported text field gains focus.',
 '使用 [b]F8[/b] 在受支持的天际文本框内输入，提供游戏内候选面板、直接英文输入与键盘热键过滤。可选自动开启可在支持的文本框聚焦时启动输入。'),
('The interface now supports [b]English, Simplified Chinese, Japanese and Korean[/b], selected automatically from the Windows display language. Other languages fall back to English. The selected IME does not change the UI language; a manual override is optional.',
 '界面现支持[b]英文、简体中文、日语和韩语[/b]，默认根据 Windows 显示语言自动识别，其他语言回退英文。所选输入法不会改变界面语言，也可手动覆盖。'),
]),
("What's new in 0.4.1 / 0.4.1 更新", [
('[b]A shorter, localized settings page.[/b] Simplified explanations and a direct sidebar entry remove unnecessary text and the extra English Settings submenu. The candidate panel, input-state labels and feedback are localized too.',
 '[b]更精简的本地化设置页。[/b] 精简说明，移除多余的英文 Settings 子目录，直接显示“输入与面板”。候选面板、输入状态和操作反馈也同步翻译。'),
('[b]Initial Japanese/Korean input handling.[/b] Adds Japanese kana/fullwidth mode preservation and Korean Hangul/Latin mode detection, with Unicode composition and candidate handling. Offline checks and font previews pass; [b]actual Japanese/Korean IMEs still need broader in-game testing[/b].',
 '[b]初步日/韩输入处理。[/b] 保留日语假名/全角状态，识别韩语 Hangul/英文模式，并处理 Unicode 组合文字与候选内容。离线检查与字体预览通过，[b]实际日/韩输入法仍需要更多游戏内测试[/b]。'),
('[b]Updates preserve your settings.[/b] Installation ZIPs now contain only the DLL and readme.txt, with [b]no INI[/b]. Fresh installations use built-in defaults; existing personal settings take priority. Legacy settings migrate in memory when no personal file exists. Startup does not write configuration.',
 '[b]更新保留个人设置。[/b] 安装 ZIP 现在只有 DLL 和 readme.txt，[b]不再附带 INI[/b]。首次安装使用内置默认值，已有个人配置优先；没有个人文件时在内存中沿用旧配置。启动不会写入配置。'),
]),
('Input support & compatibility / 输入支持与兼容性', [
('[b]Input support: Meridian UI (including Tailor), Modex, SkyUI and compatible Scaleform fields, including RaceMenu character naming.[/b] Text Bridge supplies input to these interfaces. Detecting a new field does not guarantee that every custom editor accepts text correctly.',
 '[b]输入支持：Meridian UI（包括 Tailor）、Modex、SkyUI 及兼容 Scaleform 文本框，包括 RaceMenu 角色命名。[/b] 本模组为这些界面提供输入功能。检测到新文本框不代表所有自定义编辑器都能正确接收文字。'),
('[b]Optional automatic activation[/b] covers eligible Scaleform and Meridian fields and is off by default. Password, hidden, disabled and read-only fields are excluded. [b]The console and Modex remain manual F8[/b]; opening the console also blocks automatic activation of fields underneath.',
 '[b]可选自动开启[/b]覆盖符合条件的 Scaleform 与 Meridian 字段，默认关闭，排除密码、隐藏、禁用和只读字段。[b]控制台与 Modex 保持手动 F8[/b]，控制台打开时也阻止底下残留焦点自动开启。'),
('[b]Coexistence: Prisma UI / Outfit Wheeler.[/b] Prisma uses its own native input. Text Bridge yields to it and [b]does not provide or take over IME support for Prisma interfaces[/b], or fix Prisma candidate-window behavior. Prisma is not a Text Bridge dependency.',
 '[b]兼容共存：Prisma UI / Outfit Wheeler。[/b] Prisma 使用自身原生输入，Text Bridge 主动避让，[b]不为 Prisma 界面提供或接管输入法支持[/b]，也不修复 Prisma 自身候选窗表现。使用本模组不需要安装 Prisma。'),
('[b]Disable SimpleIME[/b] and duplicate Text Bridge installations. Install each UI mod with its own required framework. Retains Modex Esc/close cleanup. Independent ImGui interfaces other than those tested are not automatically considered supported.',
 '[b]请禁用 SimpleIME[/b] 和重复的 Text Bridge 版本，各界面模组仍需要自己的前置框架。保留 Modex Esc/关闭清理，其他未测试的独立 ImGui 界面不自动视为已支持。'),
]),
('Before you start / 前置要求', [
('[b]Required: Windows and an installed IME, matching SKSE64 and Address Library, [url=https://www.nexusmods.com/skyrimspecialedition/mods/92948]Media Keys Fix SKSE[/url], and SKSE Menu Framework with the required HUD/ImGui exports.[/b] The framework is required for both the candidate panel and settings; local interface checks use version 3.18.',
 '[b]必需：Windows 与已安装的输入法、匹配的 SKSE64 和 Address Library、[url=https://www.nexusmods.com/skyrimspecialedition/mods/92948]Media Keys Fix SKSE[/url]，以及有相应 HUD/ImGui 导出的 SKSE Menu Framework。[/b] 框架同时用于候选面板和设置页，本地接口检查使用 3.18。'),
('[b]Media Keys Fix SKSE[/b] supplies the nonexclusive keyboard environment needed for Windows IME messages. Without it, the panel may open while text or candidates fail. Equivalent keyboard patches require separate verification; this prerequisite does not require Prisma UI.',
 '[b]Media Keys Fix SKSE[/b] 提供 Windows 输入法消息所需的非独占键盘环境，缺少时可能面板能打开但文字或候选不工作。等效键盘补丁需自行验证，此前置不意味着需要安装 Prisma UI。'),
('[b]Fonts are required for the languages you display.[/b] In SKSEMenuFramework.ini, enable the needed [b]EnableChinese / EnableJapanese / EnableKorean[/b] options and choose a [b]PrimaryFont containing those glyphs[/b] under Data/SKSE/Plugins/fonts. Enabling a range alone cannot supply missing glyphs. Use a multilingual CJK font for multiple languages and restart after changes. Fonts are not bundled; Text Bridge does not modify the shared framework configuration.',
 '[b]字体必须支持准备显示的语言。[/b] 在 SKSEMenuFramework.ini 启用需要的 [b]EnableChinese / EnableJapanese / EnableKorean[/b]，并设置 Data/SKSE/Plugins/fonts 下[b]包含相应字形的 PrimaryFont[/b]，仅启用范围无法补齐缺字。多语言可使用多语言 CJK 字体，修改后重启。字体不随包提供，本模组不修改共享框架配置。'),
('[b]Exact admitted runtimes: 1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104.[/b] All four pass offline address/queue/layout checks. Previous stable input was user-tested on 1.6.1170; the new multilingual behavior and other runtimes need additional game testing. Other patches, GOG 1.6.1179 and VR are not admitted.',
 '[b]精确准入版本：1.5.97 / 1.6.1170 / 1.7.99 / 1.7.104。[/b] 四版本通过离线地址/队列/布局检查，之前稳定输入已在 1.6.1170 获用户实测；新增多语言功能与其他运行时需要更多游戏测试。未放行其他补丁、GOG 1.6.1179 或 VR。'),
('Every prerequisite must independently support your game version. [b]1.7.104[/b] uses SKSE 2.3.1 and its matching Address Library; Media Keys Fix 1.0.2 documents 1.7.99, and its 1.7.104 operation remains unverified here.',
 '各前置必须独立支持你的游戏版本。[b]1.7.104[/b] 使用 SKSE 2.3.1 与对应 Address Library；Media Keys Fix 1.0.2 说明列出 1.7.99，本项目尚未确认其在 1.7.104 的工作状态。'),
]),
('Get started / 安装与使用', [
('Exit Skyrim, install the ZIP through MO2 or your mod manager, and enable one Text Bridge version. The package contains [b]only DLL and readme.txt[/b], including complete license notices. [b]Preserve SkyrimTextBridge.user.ini[/b] when replacing an installation; back it up before removing/reinstalling a mod. MO2-generated settings may be in Overwrite or a configured output mod.',
 '退出 Skyrim 后通过 MO2 或其他管理器安装 ZIP，只启用一个 Text Bridge 版本。安装包[b]只有 DLL 和 readme.txt[/b]，后者合并完整许可。替换安装时[b]保留 SkyrimTextBridge.user.ini[/b]，删除或重装模组前先备份；MO2 新生成的配置可能位于 Overwrite 或指定输出模组。'),
('Click a supported field and press [b]F8[/b]. English state accepts normal English typing; use your IME\'s own language key or optional [b]Ctrl+Space[/b] for native input. The latter cancels uncommitted composition. F8 closes the mode. A custom hotkey replaces F8 throughout these instructions.',
 '点击支持的文本框后按 [b]F8[/b]，英文状态可正常输入英文，使用输入法自身切换键或可选 [b]Ctrl+空格[/b] 切换本地语言输入；后者会取消未提交的组合文字。F8 也可关闭，自定义快捷键后用你的按键替代本文 F8。'),
('[b]Esc[/b] first cancels active composition, then exits after committed text is delivered; press again if needed. The exit press/release is consumed so it does not close Modex. Exit requests English state for gameplay without selecting another Windows input profile.',
 '[b]Esc[/b] 先取消组合文字，再于已选文字送达后退出，必要时再按一次。退出的按下/松开被拦截，避免透传关闭 Modex；退出请求英文状态以便游戏操作，不更换 Windows 输入法配置。'),
('Open [b]F1 → Skyrim Text Bridge → Input & panel[/b] (localized automatically) to configure language, enabled state, hotkey, automatic activation, position, scale, colors and opacity. Click [b]Save settings[/b] to create/update [b]SkyrimTextBridge.user.ini[/b]. Page language updates immediately; the sidebar label updates after restart. Configuration is stored outside Skyrim saves.',
 '打开 [b]F1 → Skyrim Text Bridge → 输入与面板[/b]（名称自动本地化），调整语言、开关、快捷键、自动开启、位置、缩放、颜色和透明度。点击[b]保存设置[/b]才创建/更新 [b]SkyrimTextBridge.user.ini[/b]。页面语言即时更新，侧栏名称重启后更新；配置独立于 Skyrim 存档保存。'),
('Automatic activation is off by default. Enable it and save to use focused fields. Manual F8/Esc exit suppresses immediate reactivation until focus leaves and returns. Automatic sessions end on blur; menu changes, loading saves, opening settings and losing game focus also end input.',
 '自动开启默认关闭，在设置页启用并保存。手动 F8/Esc 退出后，当前焦点不会立即重开，离开再聚焦才恢复；自动会话失焦后结束，切换菜单、读档、打开设置和游戏失焦也会退出输入。'),
]),
('Things to know / 使用须知', [
('The in-game ImGui panel displays composition, IME-provided candidates and the current Windows input method name. Position, size, colors and background opacity are adjustable. [b]Emoji rendering is unsupported[/b]; the destination editor needs its own font/Unicode support too. Missing interface glyphs temporarily use English settings help; candidates still require the relevant font.',
 '游戏内 ImGui 面板显示组合文字、输入法提供的候选及当前 Windows 输入法名称，可调位置、大小、颜色和背景透明度。[b]不支持 Emoji 渲染[/b]，目标编辑器也需自身的字体/Unicode 支持。界面缺字时设置页暂用英文保证可操作，候选仍需要相应字体。'),
('Some IMEs do not expose IMM candidate lists. Japanese clause conversion, Korean Hanja candidates and third-party IMEs need real-engine testing; simulated composition checks do not establish universal IME support. The mod does not install an IME or translate typed text.',
 '部分输入法不提供 IMM 候选列表。日语分节转换、韩语 Hanja 候选及第三方输入法需真实测试，模拟组合检查不代表支持所有输入法。本模组不安装输入法、不翻译输入文字。'),
('Hotkey filtering applies to ordinary downstream keyboard events while active. Mouse and editing keys remain usable; Ctrl+A/C/X/V pass through outside composition when supported by the editor. Earlier hooks, independent polling and hotkeys bound to editing keys may still react. Prisma input is not filtered.',
 '热键过滤作用于输入期间后续游戏输入链的普通键事件，保留鼠标和编辑键；非组合期间允许 Ctrl+A/C/X/V 通过，取决于编辑器。较早钩子、独立轮询和绑定在编辑键的热键仍可能响应，Prisma 输入不应用此过滤。'),
('The panel is rendered in-game. True exclusive fullscreen, Alt+Tab and rendering-mod combinations still require game validation. No private rendering device, desktop overlay or emoji fallback is created.',
 '面板在游戏内绘制，真正独占全屏、Alt+Tab 和渲染模组组合仍需游戏验证。不创建私有绘图设备、桌面悬浮窗或 Emoji 补绘。'),
]),
('Open source & feedback / 开源与反馈', [
('Original code is [b]GPL-3.0-only[/b]; third-party files retain their licenses, included in readme.txt. [url=https://github.com/BlackMesa79/Skyrim-Text-Bridge]GitHub hosts the source[/url]; installation packages are distributed on Nexus. Matching source and symbols are separate archives. Reports should include game version, IME, related UI mod versions, reproduction steps and SkyrimTextBridge.log. Entered text is not logged.',
 '自身代码使用 [b]GPL-3.0-only[/b]，第三方保留许可，完整说明合并在 readme.txt。[url=https://github.com/BlackMesa79/Skyrim-Text-Bridge]GitHub 托管源码[/url]，模组安装包在 Nexus 分发，对应源码与符号另包。反馈请提供游戏版本、输入法、界面模组版本、复现步骤与 SkyrimTextBridge.log，日志不记录输入文字。'),
]),
]

intro = [('Chinese, Japanese and Korean IME Support', '中文、日语与韩语输入法支持'),
         ('In-game candidates. Direct English typing. Optional auto activation.', '游戏内候选面板 · 直接英文输入 · 可选自动开启')]
bb = ['[center][size=6][b]Skyrim Text Bridge[/b][/size]', '[color=#C9AA72][b]' + intro[0][0] + '[/b][/color]', '[color=#C9AA72]' + intro[0][1] + '[/color]', '', intro[1][0], '', intro[1][1] + '[/center]']
md = ['# Skyrim Text Bridge', '', '**' + intro[0][0] + '**', '', intro[0][1], '', intro[1][0], '', intro[1][1]]

def markdown(text):
    text = re.sub(r'\[url=([^\]]+)\](.*?)\[/url\]', r'[\2](\1)', text)
    return text.replace('[b]', '**').replace('[/b]', '**')

for heading, pairs in sections:
    bb += ['', '[size=4][color=#C9AA72][b]' + heading + '[/b][/color][/size]']
    md += ['', '## ' + heading]
    for en, zh in pairs:
        bb += ['', en, '', '[color=#B8C1C8]' + zh + '[/color]']
        md += ['', markdown(en), '', markdown(zh)]

bbcode = '\n'.join(bb) + '\n'
(out / 'description-en-zh.bbcode.txt').write_text(bbcode, encoding='utf-8-sig')
(out / 'description-en-zh.md').write_text('\n'.join(md) + '\n', encoding='utf-8')
body = html.escape(bbcode)
body = re.sub(r'\[url=([^\]]+)\](.*?)\[/url\]', r'<a href="\1">\2</a>', body)
body = re.sub(r'\[size=(\d+)\]', r'<span class="size\1">', body).replace('[/size]', '</span>')
body = re.sub(r'\[color=(#[0-9A-Fa-f]+)\]', r'<span style="color:\1">', body).replace('[/color]', '</span>')
body = body.replace('[b]', '<strong>').replace('[/b]', '</strong>').replace('[center]', '<div class="center">').replace('[/center]', '</div>').replace('\n', '<br>\n')
assert not re.search(r'\[/?(?:color|size|b|url|center)', body)
preview = '''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Skyrim Text Bridge 0.4.1 — Nexus preview</title><style>body{background:#13191d;color:#d6dbde;font:16px/1.8 "Segoe UI","Microsoft YaHei",sans-serif;margin:0}main{max-width:860px;margin:auto;padding:48px 28px}.center{text-align:center}.size6{font-size:34px}.size4{font-size:23px}a{color:#dbc086}strong{color:#f2f2ec}span[style] strong{color:inherit}@media(max-width:600px){main{padding:30px 18px}.size6{font-size:29px}.size4{font-size:21px}body{font-size:15px}}</style><main>'''
(out / 'Nexus-preview.html').write_text(preview + body + '</main></html>', encoding='utf-8')

changelog = '''0.4.1

Added English, Simplified Chinese, Japanese and Korean UI, automatically following Windows display language; shortened settings help and removed the extra Settings submenu.
加入自动跟随 Windows 显示语言的英、简中、日、韩界面，精简设置说明，移除多余 Settings 子目录。

Added initial Japanese/Korean IME handling, including kana/fullwidth preservation and Hangul mode detection. Actual IMEs still need broader game testing.
加入初步日/韩输入处理，包括假名/全角保留与 Hangul 状态识别；实际输入法仍需更多游戏测试。

Installation ZIP now contains only DLL and readme.txt. Built-in defaults work without INI; startup preserves personal settings and fully reads legacy settings when needed.
安装 ZIP 仅含 DLL 和 readme.txt；无 INI 时使用内置默认值，启动保留个人配置，并在需要时完整读取旧配置。
'''
(out / 'changelog-en-zh.txt').write_text(changelog, encoding='utf-8-sig')
readme = '''SKYRIM TEXT BRIDGE 0.4.1
By BlackMesa79
https://github.com/BlackMesa79/Skyrim-Text-Bridge

Chinese Japanese and Korean IME Support / 中文、日语与韩语输入法支持

'''
# Keep the installed readme concise and retain full licensing appended by Package.ps1.
for index in (0, 2, 3, 4, 5):
    heading, pairs = sections[index]
    readme += heading + '\n'
    for en, zh in pairs:
        plain = lambda value: re.sub(r'\[url=([^\]]+)\](.*?)\[/url\]', r'\2 (\1)', value).replace('[b]', '').replace('[/b]', '')
        readme += plain(en) + '\n' + plain(zh) + '\n\n'
readme += '''LICENSE AND SOURCE / 许可与源码
Copyright (C) 2026 BlackMesa79. Original code: GPL-3.0-only, without warranty. Third-party files retain their original licenses; complete notices follow. Corresponding source and build instructions: SkyrimTextBridge-0.4.1-source.zip. Provide corresponding source when redistributing binaries. SKSE Menu Framework is installed separately. No restriction is added against modifications or reverse engineering for debugging modifications to an LGPL component.
Copyright (C) 2026 BlackMesa79。自身代码采用 GPL-3.0-only，不提供担保；第三方保留原许可，完整声明附于下方。对应源码与构建说明：SkyrimTextBridge-0.4.1-source.zip，转发二进制须提供对应源码。SKSE Menu Framework 另行安装，不额外限制为调试 LGPL 组件修改而进行的修改或逆向工程。
'''
(out / 'readme-intro.txt').write_text(readme, encoding='utf-8-sig')

# Retain the established charcoal/ivory/gold vector template.
svg = (root / 'release-materials/0.3.7/SkyrimTextBridge-cover.svg').read_text(encoding='utf-8-sig')
svg = svg.replace('A keyboard feeds Chinese text into a field.', 'A keyboard feeds Chinese, Japanese and Korean text into a field.')
svg = svg.replace('Chinese input for Skyrim.', 'Chinese · Japanese · Korean IME')
svg = svg.replace('font-size="47">Chinese', 'font-size="42">Chinese')
svg = svg.replace('天际中文输入支持', '中文 · 日语 · 韩语输入法支持')
svg = svg.replace('y="376" width="734" height="270"', 'y="310" width="734" height="425"')
svg = svg.replace('M1685 462 H2324', 'M1685 392 H2324')
svg = svg.replace('M2295 503 V593 M2280 503 H2310 M2280 593 H2310', 'M2295 638 V695 M2280 638 H2310 M2280 695 H2310')
svg = svg.replace('M1920 812 V759 Q1920 719 1960 719 H2074 Q2114 719 2114 680', 'M1920 812 V799 Q1920 777 1960 777 H2074 Q2114 777 2114 754')
svg = svg.replace('M2094 700 L2114 680 L2134 700', 'M2094 774 L2114 754 L2134 774')
svg = svg.replace('y="435"', 'y="368"').replace('ni hao tian ji', 'CHINESE / JAPANESE / KOREAN').replace('font-size="34" letter-spacing="3"', 'font-size="25" letter-spacing="2"')
svg = svg.replace('<text x="1700" y="590" fill="#f4f1e8" font-size="92" font-family="Microsoft YaHei, sans-serif" letter-spacing="8">你好，天际</text>', '''<text x="1700" y="493" fill="#f4f1e8" font-size="74" font-family="Microsoft YaHei, sans-serif">你好，天际</text>
  <text x="1700" y="590" fill="#f4f1e8" font-size="70" font-family="Yu Gothic, Meiryo, sans-serif">こんにちは</text>
  <text x="1700" y="688" fill="#f4f1e8" font-size="70" font-family="Malgun Gothic, sans-serif">안녕하세요</text>''')
svg = svg.replace('</svg>', '''<text x="170" y="1265" fill="#728180" font-size="28" font-family="Segoe UI, sans-serif" letter-spacing="3">IN-GAME CANDIDATES  /  OPTIONAL AUTO ACTIVATION</text>
</svg>''')
(out / 'SkyrimTextBridge-cover.svg').write_text(svg, encoding='utf-8')
print(f'Generated 0.4.1 materials. Summary: EN {len(summary_en)}, ZH {len(summary_zh)}, combined {len(summary_en + summary_zh)+2} characters.')
