# 0.3.11-beta.1 runtime experiment

Branch: `codex/runtime-1-7-104`. Local testing only; public beta is deferred until the user confirms local regression results. The 0.3.10 stable checkout and deployed DLL are preserved.

## Changes

- Pin CommonLibSSE-NG 10.1.0 (`39f9d07a6ffabea8fb559eee87ab7d27cd463e8a`) with matching locally compiled SE+AE/non-VR static library, multi-targeting and patch safety.
- Admit exact 1.7.104 alongside 1.5.97 / 1.6.1170; reject 1.7.99, 1.7.105 and nonzero build variants.
- Advertise Address Library V5 support in SKSE metadata and use upstream version-aware pool, head and tail accessors. Legacy character pool is at 0x200 and queue head/tail at 0x380/0x388; 1.7 pool is at 0x208 and head/tail at 0x558/0x560.
- Keep existing IME behavior, UI integrations and settings. Package and deployment defaults now name the isolated beta.

## Completed offline validation

Full Windows/MSVC build passed on 2026-10-04: 60 core, 43 direct IME and 52 runtime queue/address assertions, plus Meridian DOM/ABI, Prisma V1, input method name, settings and DLL metadata checks. New runtime checks load actual address files with CommonLib's real loader and use mock engine memory; cover queue restoration, English/Unicode order, pool capacity, 1.7 extra-pool sentinels and ControlMap access.

| Runtime | Address format | Dispatch ID | Dispatch RVA |
| --- | --- | --- | --- |
| 1.5.97 | 1 | 67315 | 0xC150B0 |
| 1.6.1170 | 2 | 68617 | 0xCD8F40 |
| 1.7.104 | 5 | 68617 | 0xCF95F0 |

DLL SHA256: `C495F086455302C9E171ACDF941C8642C5F87A9328C29DD00CC148E8CE0799BA`.
Stable DLL SHA256: `A3ED0CCEFACA9028D739538C078E26E63B9E8DF9E2251421E3D77335D8201A96`.

These are not game-test results. No 1.7.104 executable is available locally. The dispatch hook still uses the AE +0x7B call offset and checks E8 before patching; current Meridian upstream independently uses the same AE offset. Actual 1.7.104 instruction placement, hook chaining and input behavior still need game validation. The 0.3.10 user feedback does not validate the changed dependency/binary on old runtimes.

## Prerequisites

Official SKSE lists 2.3.1 for Steam Skyrim 1.7.104. Use its matching Address Library. Media Keys Fix SKSE or an equivalent working nonexclusive keyboard patch remains required. The locally installed Media Keys Fix 1.0.2 description advertises 1.7.99, not 1.7.104; its upstream build pins CommonLib `e7863a71523a2896c92ea9d3105c0d121dcdba0d`. A read-only PE metadata audit found StructCompatibility flags 1 (no V5 flag), while the new Text Bridge has flags 3 (V5 present). This alone does not prove Media Keys Fix cannot load: current SKSE also considers build timestamps for this compatibility flag. Neither its installed binary nor the optional UI frameworks have been tested on 1.7.104 here. This is an unverified dependency, not a demonstrated incompatibility.

## Game checklist / 游戏检查

先在现有 1.6.1170 环境只启用实验版，禁用正式版与 SimpleIME。无需升级当前游戏。通过后，再由 1.7.104 用户使用匹配前置测试。切勿同时启用两个 Text Bridge。

1. 启动无错误，SKSE 日志记录实验版本和当前游戏版本，F1 设置页可见（需对应可用的 Menu Framework）。
2. SkyUI 搜索框按 F8，中文候选及结果正确，Ctrl+空格英文输入正常；Modex 手动输入正常。
3. Meridian/Tailor 中文和英文均送达目标框；打开可选自动开启后聚焦、失焦、手动关闭行为正常。
4. Esc 先取消组合；输入完成后退出。离开 UI 后重新按 WASD 可移动，不出现拼音残留；再开启输入可继续使用。
5. Prisma/Outfit Wheeler 保留原生输入，Text Bridge 不接管；Alt+Tab、关菜单、读档不残留输入状态。
6. 失败时记录游戏、SKSE、地址库及各前置版本，检查 SkyrimTextBridge.log 和 skse64.log。若 Text Bridge 成功加载但完全收不到 IME 输入，先核对非独占键盘前置是否实际工作。

回退：退出游戏，禁用独立测试模组，重新启用 0.3.10；不修改存档。
