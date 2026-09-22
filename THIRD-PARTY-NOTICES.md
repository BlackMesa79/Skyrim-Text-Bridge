# Third-party notices

The GPL-3.0-only license applies to original Skyrim Text Bridge code. Third-party source files, SDK headers and their licenses remain separate and retain their original notices.

- **CommonLibVR / CommonLibSSE** (`extern/CommonLibVR`): MIT. Matching source snapshot used by the local build, including upstream build scripts. Upstream: https://github.com/alandtse/CommonLibVR . Copyright notices are in its LICENSE and source files.
- **spdlog 1.16.0** (`extern/spdlog`): MIT; retained per-file notices also apply to bundled headers. Upstream: https://github.com/gabime/spdlog . Only sources/headers and build support are included, not a compiled library.
- **Meridian UI public SDK** (`extern/MeridianUI`): ViewAPI.h and Settings.h explicitly carry SPDX MIT notices. The upstream project LICENSE is GPL-3.0 and is included unchanged; no Meridian renderer or CEF binaries are distributed. Upstream: https://github.com/heathbrownkeyworks/MeridianUI .
- **SKSE Menu Framework API** (`extern/SKSEMenuFramework`): unmodified API header and upstream LGPL-2.1 license. The runtime is installed separately. Upstream: https://github.com/QTR-Modding/SKSE-Menu-Framework-3 .
- **Prisma UI API** (`extern/PrismaUI`): unmodified public API header, whose introductory notice permits modders to copy it into their projects. The upstream custom license is included unchanged. It is not GPL-relicensed. No Prisma framework implementation or Ultralight binaries are distributed. Upstream: https://github.com/PrismaUI-SKSE/framework .

Skyrim, SKSE, framework DLLs and rendering runtimes are external components. Install them separately under their respective terms. This repository does not grant rights to those components.
