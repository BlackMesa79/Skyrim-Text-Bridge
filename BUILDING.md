# Building Skyrim Text Bridge

## Validated toolchain

Windows x64, Visual Studio 2022 C++ toolset 14.44, Windows SDK 10.0.26100.0, PowerShell, Node.js, and spdlog 1.16.0. `scripts/Build-Local.ps1` discovers Visual Studio/SDK through vswhere and the registry.

The experimental branch pins CommonLibSSE-NG 10.1.0 to `39f9d07a6ffabea8fb559eee87ab7d27cd463e8a` under the historical `extern/CommonLibVR` path. Matching public framework headers are included. No prebuilt CommonLib `.lib` or framework/game DLLs are included. Upstream's current GPL license and exceptions are retained; see THIRD-PARTY-NOTICES.md.

## Build CommonLib first

Use xmake 3.x from a Visual Studio x64 tools environment:

```powershell
xmake f -P extern/CommonLibVR -a x64 -m releasedbg --skyrim_se=y --skyrim_ae=y --skyrim_vr=n --tests=n --rex_ini=n --rex_json=n --rex_toml=n --skse_xbyak=n --skse_patch_safety=y -y
xmake build -P extern/CommonLibVR commonlibsse-ng
```

xmake downloads the dependencies declared by the vendored CommonLib build file. Keep its source-built SE+AE, non-VR configuration and release CRT `/MD`; the upstream all-runtime/VR prebuilt package is not a substitute. Locate the resulting `commonlibsse-ng.lib` under its build directory and pass that exact path below. Do not overwrite headers with another CommonLib revision.

The experimental dependency was built from the pinned source with this configuration. The plugin uses matching headers, multi-targeting and patch-safety flags. An empty dependency-cache rebuild has not been performed.

## Build and verify the plugin

```powershell
./scripts/Build-Local.ps1 -CommonLibArchive '<full path to commonlibsse-ng.lib>'
```

The default archive path is `build/windows/x64/releasedbg/commonlibsse-ng.lib`, as produced by the command above in the validated toolchain. The script uses vendored spdlog sources, generates the Meridian JS header, runs the native/DOM regression checks, compiles and links the plugin, then validates SKSE metadata, including the V5 address-library flag. Output: `build/native/SkyrimTextBridge.dll`.

To also check actual Address Library files and runtime queue layouts without starting Skyrim:

```powershell
./scripts/Build-Local.ps1 -RuntimeAddressLibrary '<folder containing the 1.5.97, 1.6.1170 and 1.7.104 address files>'
```

This optional checker uses CommonLib's real address loader and mock runtime memory. It needs no game executable; missing address files fail the check. It does not validate executable hook instructions or in-game IME behavior.

These checks do not launch Skyrim; supported runtime metadata is not a claim of game testing on any of the three runtimes with this experimental binary.

## Packaging

```powershell
./scripts/Package.ps1 -Release
```

Produces `dist/SkyrimTextBridge-0.3.11-beta.1.zip` (exactly the DLL, default INI and one readme.txt containing usage and full license notices) and a separate symbols ZIP. `-Release` chooses the package filename and includes symbols; it does not publish or tag anything. This remains an experimental local test package. Prepare its corresponding source ZIP with `git archive` from the same source commit before distributing it.

`Sync-Mod.ps1` is an optional local deployment helper. Supply `-Target '<your MO2 mod folder>' -Release`; it refuses to replace a running game's DLL and backs up overwritten files. The folder must already exist. On this branch the default is the separate `36 - SkyrimTextBridge - 1.7.104 Test` mod folder; the stable installation is not replaced. Enable only one Text Bridge package in MO2. Existing user settings can be copied separately if desired.

Root `xmake.lua` is an alternative development build; the release validation path is `Build-Local.ps1`.
