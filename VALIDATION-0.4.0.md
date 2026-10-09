# 0.4.0 release validation — 2026-10-09

The user confirmed `0.3.12-beta.8` works well in the local 1.6.1170 environment and requested release `0.4.0`, a compatibility recheck, updated bilingual publication materials and GitHub publication. Compared with beta.8, production input/rendering logic, default configuration and runtime admission are unchanged; only plugin version and logging change. New assertions extend runtime compatibility verification, without adding runtime behavior.

用户确认本地 1.6.1170 的 `0.3.12-beta.8` 可用，要求以 `0.4.0` 发布、复核运行时兼容性、更新双语详情及推送 GitHub。与 beta.8 相比，生产输入/绘制逻辑、默认配置及版本准入保持一致，仅修改插件版本和日志；新增断言扩展兼容性验证，不增加运行时行为。

## Exact runtime admission / 精确版本范围

| Runtime | Offline evidence | Game validation |
| --- | --- | --- |
| 1.5.97.0 | SE address library, character pool/queue links, ControlMap and UI/IMenu layouts, focus and cleanup relocations passed | Current build not fully game-tested |
| 1.6.1170.0 | AE address library and corresponding layouts/relocations passed | User confirmed beta.8 behavior and approved 0.4.0 promotion |
| 1.7.99.0 | V5 address library, moved character/queue fields, protected new event pools, UI/IMenu and ControlMap layouts/relocations passed | Full game validation pending |
| 1.7.104.0 | V5 address library and corresponding updated layouts/relocations passed | Full game validation pending |

Both `SupportedRuntime` and SKSE plugin metadata admit only these four exact versions with build zero. Other 1.5/1.6/1.7 patches, GOG 1.6.1179 and VR are not admitted. Offline success verifies the pinned CommonLib implementation against address files and simulated memory, not every reverse-engineered layout against a running executable. Installation still checks the dispatch hook's E8 opcode; the remote runtime's executable hook site has not been run here.

代码与 SKSE 插件元数据仅放行这四个精确版本（build 为零），未扩大到其他 1.5/1.6/1.7 补丁、GOG 1.6.1179 或 VR。离线通过代表固定 CommonLib 实现与地址文件、模拟内存检查通过，不代表每项逆向布局均已在运行的游戏中确认。安装时仍检查派发钩子 E8 指令，本地未执行其他运行时的实际钩子位置。

## Build and checks / 构建与检查

CommonLibSSE-NG 10.1.0 pinned at `39f9d07a6ffabea8fb559eee87ab7d27cd463e8a`; matching source-built SE+AE, non-VR archive, MSVC /MD, multi-targeting and patch safety. Full `Build-Local.ps1` release rebuild passed:

- 66 core assertions, including strict accepted/rejected runtime checks.
- 102 production framework/Scaleform focus assertions, including console exclusion and underlying focus blocking.
- 26 Modex menu guard assertions; 60 DirectIme lifecycle assertions.
- 14 real isolated TSF state checks; input-method names, Prisma negotiation and settings checks.
- 32 ImGui panel assertions; Meridian DOM and public ABI checks.
- 90 four-runtime queue/address/layout assertions. Extended checks resolve `ControlMap::AllowTextInput` (SE 67252 / AE 68552) for Modex lease cleanup and verify the shared non-VR UI/IMenu fields used by focus/console detection.
- DLL Query/Load exports, 0.4.0 packed metadata, V5 flag and exact compatibility list.

All address checks use the production CommonLib loader on the four locally installed address-library files. No game entry point is executed. Exclusive fullscreen, newly detected editors and alternative IME layouts remain subject to game validation. Emoji support was removed at user request; panel text uses framework fonts and normal Unicode delivery is retained.

地址检查使用生产 CommonLib 加载器读取本地四版本地址库，未执行游戏入口。独占全屏、新识别编辑器及其他输入法布局仍需游戏验证。按用户要求取消 Emoji 支持，面板使用框架字体，保留正常 Unicode 传递。

DLL SHA256: `46983F880BBAEED5787B04E707D6D1C2A7C902B8A1EA02C877B99932A81E6F53`.

## Prerequisites and publication / 前置与发布

Matching SKSE64 and Address Library, Media Keys Fix SKSE or a verified equivalent nonexclusive keyboard patch, and SKSE Menu Framework with public HUD/ImGui exports and a configured Chinese font are required. Each dependency must independently support the runtime. Local framework public API checks used 3.18. 1.7.104 uses SKSE 2.3.1; Media Keys Fix 1.0.2 documents 1.7.99, and its 1.7.104 operation remains unconfirmed here. Prisma is a coexistence target, not a dependency or input backend supplied by Text Bridge.

需要匹配的 SKSE64 与 Address Library、Media Keys Fix SKSE 或已验证等效非独占键盘补丁，以及提供公开 HUD/ImGui 接口且配置中文字体的 SKSE Menu Framework，各依赖需独立兼容游戏版本。本地框架接口检查为 3.18；1.7.104 使用 SKSE 2.3.1，Media Keys Fix 1.0.2 文档列出 1.7.99，其在 1.7.104 的表现尚未确认。Prisma 属于避让共存对象，不是本模组前置或本模组提供输入的后端。

Installation ZIP contains exactly DLL, default INI and one readme.txt with full usage and license texts. Matching source and symbols are separate. GPL-3.0-only and third-party licenses are retained. MO2 sync backs up replacements, refuses while Skyrim is running and preserves personal configuration and metadata. Original workspace's preexisting source changes are not overwritten. Source is promoted to GitHub main by a normal fast-forward push, with an annotated v0.4.0 tag and a normal GitHub Release; no history rewrite is needed.

安装 ZIP 仅含 DLL、默认 INI 及合并使用说明与完整许可的一个 readme.txt，对应源码/符号另包；保留 GPL-3.0-only 和第三方许可。MO2 同步备份替换文件、游戏运行时拒绝同步、保留个人配置及元数据，不覆盖原工作目录的已有源码修改。源码通过正常快进推送进入 GitHub main，创建 v0.4.0 注释标签与普通 GitHub Release，无需改写历史。
