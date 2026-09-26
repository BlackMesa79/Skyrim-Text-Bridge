# Building Skyrim Text Bridge

## Validated toolchain

Windows x64, Visual Studio 2022 C++ toolset 14.44, Windows SDK 10.0.26100.0, PowerShell, Node.js, and spdlog 1.16.0. `scripts/Build-Local.ps1` discovers Visual Studio/SDK through vswhere and the registry.

The repository contains the matching CommonLib source snapshot and public framework headers. No prebuilt CommonLib `.lib` or framework/game DLLs are included.

## Build CommonLib first

Use xmake 3.x from a Visual Studio x64 tools environment:

```powershell
xmake f -P extern/CommonLibVR -a x64 -m releasedbg --skyrim_se=y --skyrim_ae=y --skyrim_vr=n --tests=n --rex_ini=n --rex_json=n --rex_toml=n --skse_xbyak=n
xmake build -P extern/CommonLibVR commonlibsse-ng
```

xmake downloads the dependencies declared by the vendored CommonLib build file. Keep its source-built SE+AE, non-VR configuration and release CRT `/MD`; the upstream all-runtime/VR prebuilt package is not a substitute. Locate the resulting `commonlibsse-ng.lib` under its build directory and pass that exact path below. Do not overwrite headers with another CommonLib revision.

The production plugin build has been tested with a matching locally built static library. The dependency rebuild instructions have not been rerun from an empty dependency cache for this release.

## Build and verify the plugin

```powershell
./scripts/Build-Local.ps1 -CommonLibArchive '<full path to commonlibsse-ng.lib>'
```

Alternatively place the matching library at `extern/CommonLibVR/lib/commonlibsse-ng.lib`. The script uses vendored spdlog sources, generates the Meridian JS header, runs the native/DOM regression checks, compiles and links the plugin, then validates SKSE metadata. Output: `build/native/SkyrimTextBridge.dll`.

These checks do not launch Skyrim; supported runtime metadata is not a claim of game testing on both runtimes.

## Packaging

```powershell
./scripts/Package.ps1 -Release
```

Produces `dist/SkyrimTextBridge-0.3.6.zip` and a separate symbols ZIP, with licenses and documentation. Source is available from the repository alongside each source commit. Do not use prototype packages for normal public distribution.

`Sync-Mod.ps1` is an optional local deployment helper. Supply `-Target '<your MO2 mod folder>' -Release`; it refuses to replace a running game's DLL and backs up overwritten files. Its default target is the developer's local installation, not a required path.

Root `xmake.lua` is an alternative development build; the release validation path is `Build-Local.ps1`.
