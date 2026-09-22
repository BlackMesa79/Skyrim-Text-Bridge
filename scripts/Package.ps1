param([string]$Version = '0.3.5', [switch]$Release)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$dist = Join-Path $projectRoot 'dist'
$suffix = if ($Release) { '' } else { '-prototype' }
$stage = Join-Path $dist "SkyrimTextBridge-$Version$suffix"
New-Item -ItemType Directory -Path (Join-Path $stage 'SKSE\Plugins') -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $stage 'Documentation\SkyrimTextBridge\Licenses') -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $projectRoot 'build\native\SkyrimTextBridge.dll') -Destination (Join-Path $stage 'SKSE\Plugins')
if (!$Release) { Copy-Item -LiteralPath (Join-Path $projectRoot 'build\native\SkyrimTextBridge.pdb') -Destination (Join-Path $stage 'SKSE\Plugins') }
Copy-Item -LiteralPath (Join-Path $projectRoot 'package\SKSE\Plugins\SkyrimTextBridge.ini') -Destination (Join-Path $stage 'SKSE\Plugins')
Copy-Item -LiteralPath (Join-Path $projectRoot 'README.md') -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\README.md')
Copy-Item -LiteralPath (Join-Path $projectRoot "VALIDATION-$Version.md") -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\VALIDATION.md')
Copy-Item -LiteralPath (Join-Path $projectRoot 'extern\CommonLibVR\LICENSE') -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\Licenses\CommonLibVR.txt')
Copy-Item -LiteralPath (Join-Path $projectRoot 'extern\PrismaUI\LICENSE.md') -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\Licenses\PrismaUI.md')
Copy-Item -LiteralPath (Join-Path $projectRoot 'extern\MeridianUI\LICENSE') -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\Licenses\MeridianUI.txt')
Copy-Item -LiteralPath (Join-Path $projectRoot 'extern\SKSEMenuFramework\LICENSE') -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\Licenses\MenuFramework.txt')
Copy-Item -LiteralPath (Join-Path $projectRoot 'extern\SKSEMenuFramework\SKSEMenuFramework.h') -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\Licenses\SKSEMenuFramework.h')
$spdlog = Get-Item -LiteralPath (Join-Path $projectRoot 'extern\spdlog\LICENSE') -ErrorAction SilentlyContinue
if (!$spdlog) { $spdlog = Get-ChildItem -Path (Join-Path $projectRoot 'tools-cache\xmake\.xmake\cache\packages\*\s\spdlog\v1.16.0\source\spdlog\LICENSE') | Select-Object -First 1 }
if (!$spdlog) { throw 'spdlog license not found' }
Copy-Item -LiteralPath $spdlog.FullName -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\Licenses\spdlog.txt')
foreach ($name in @('LICENSE','THIRD-PARTY-NOTICES.md')) {
    Copy-Item -LiteralPath (Join-Path $projectRoot $name) -Destination (Join-Path $stage "Documentation\SkyrimTextBridge\$name")
}
$notes = Join-Path $projectRoot "RELEASE-NOTES-$Version.md"
if (Test-Path -LiteralPath $notes) { Copy-Item -LiteralPath $notes -Destination (Join-Path $stage 'Documentation\SkyrimTextBridge\RELEASE-NOTES.md') }
$zip = Join-Path $dist "SkyrimTextBridge-$Version$suffix.zip"
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zip -Force
if ($Release) {
    Compress-Archive -LiteralPath (Join-Path $projectRoot 'build\native\SkyrimTextBridge.pdb') -DestinationPath (Join-Path $dist "SkyrimTextBridge-$Version-symbols.zip") -Force
}
Get-FileHash -LiteralPath $zip -Algorithm SHA256 | Format-List
