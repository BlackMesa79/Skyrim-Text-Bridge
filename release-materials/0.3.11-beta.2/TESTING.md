# 0.3.11-beta.2 runtime experiment — 2026-10-05

Branch remains `codex/runtime-1-7-104`. Public beta is deferred until the user tests this updated build. Stable 0.3.10 and the previous beta.1 ZIP remain available.

## Changes from beta.1

- Add exact 1.7.99 to the runtime guard and SKSE compatible-version list. Preserve 1.5.97, 1.6.1170 and 1.7.104. Reject surrounding 1.7 versions and nonzero build variants.
- Retain pinned CommonLibSSE-NG 10.1.0 and V5 metadata. Its character-pool/queue accessors switch at 1.7.99, so this runtime uses the existing new-layout path (character pool 0x208, queue head/tail 0x558/0x560).
- Add real 1.7.99 address-file loading to the offline runtime fixtures. Also check Scaleform object reference/invoke addresses and the version-specific IsValidName address on all four runtimes. 1.7.99/104 use ID 524270, replacing legacy AE ID 82331.
- Increase the test package/metadata version to 0.3.11-beta.2 (packed SKSE version 0.3.11.2). Input behavior and settings remain the same as beta.1.

## Validation and evidence limits

Beta.1 was reported by the user to work normally on local 1.6.1170. This feedback does not validate beta.2 or either 1.7 runtime.

Beta.2 full Windows/MSVC build: 61 core checks, 43 direct IME checks, 78 runtime queue/address assertions plus Meridian DOM/ABI, Prisma V1, input method name, settings and DLL metadata checks. Runtime fixtures use CommonLib's real address loader and mock engine memory. No game entry points are executed.

| Runtime | Address format | Dispatch ID | Dispatch RVA |
| --- | --- | --- | --- |
| 1.5.97 | 1 | 67315 | 0xC150B0 |
| 1.6.1170 | 2 | 68617 | 0xCD8F40 |
| 1.7.99 | 5 | 68617 | 0xCF9390 |
| 1.7.104 | 5 | 68617 | 0xCF95F0 |

Queue checks cover English/Unicode order, capacity limits, preservation of the engine event chain, runtime layout, 1.7 additional event-pool sentinels and ControlMap text-focus layout. DLL checks require exactly four allowed runtimes, beta.2 version, V5 support and Query/Load exports.

No local 1.7.99/104 executable is available. The input hook retains AE call offset +0x7B, matching current Meridian upstream, and refuses installation if it is not E8. Actual hook instructions, hook chaining and end-to-end game input still need testing. An E8 check alone does not prove the target call's identity.

## Prerequisites and installation

Use SKSE and Address Library matching the exact game version. 1.7.104 uses SKSE 2.3.1. 1.7.99 requires its own matching SKSE version and `versionlib-1-7-99-0.bin`. Media Keys Fix SKSE or an equivalent working nonexclusive keyboard patch remains required. Locally installed Media Keys Fix 1.0.2 advertises 1.7.99; that advertised support is not a local game test. Its 1.7.104 operation remains unverified. Optional Menu Framework, Meridian and Prisma also require suitable runtime builds.

Deployment reuses the existing separate MO2 folder `36 - SkyrimTextBridge - 1.7.104 Test` (historical name, now covers both targeted 1.7 runtimes), preserving its user settings. Do not enable another Text Bridge package at the same time. Stable folder and ZIP are not replaced. Installation ZIP has exactly DLL, default INI and readme.txt, with full license notices; source and symbols are separate.

## 游戏测试

1. 现有 1.6.1170 环境只启用测试模组，禁用正式版与 SimpleIME，检查无加载错误、F8 启停和可选 F1 设置页。
2. SkyUI、Modex、Meridian/Tailor 检查中文候选、提交、英文切换及编辑；自动开启下检查聚焦/失焦和手动关闭。
3. Esc 取消组合、完成后退出；返回游戏重新按 WASD 可移动，无拼音残留，再开启可输入。
4. Prisma/Outfit Wheeler 仍使用原生输入；Alt+Tab、关闭菜单和读档不残留输入状态。
5. 1.7.99 和 1.7.104 用户分别使用匹配前置重复以上测试。加载或输入失败时提供 SkyrimTextBridge.log、skse64.log 及前置版本，先排查非独占键盘补丁是否实际工作。

回退：退出游戏，将独立测试模组切回之前的 beta.1 包，或禁用测试模组并重新启用正式版 0.3.10。不修改存档。

Built DLL SHA256: `1768B9DF4A261E2370DCF9748AD599B2D859A778B92C8C29C3916D6A9986F90F`.
