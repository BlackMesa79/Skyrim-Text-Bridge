param([string]$Version = '0.3.7', [switch]$Release)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$dist = Join-Path $projectRoot 'dist'
$suffix = if ($Release) { '' } else { '-prototype' }
# Always stage in a new directory, so older documentation can never leak into a ZIP.
$stage = Join-Path $projectRoot ('build\package-' + $Version + '-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path (Join-Path $stage 'SKSE\Plugins') -Force | Out-Null
New-Item -ItemType Directory -Path $dist -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $projectRoot 'build\native\SkyrimTextBridge.dll') -Destination (Join-Path $stage 'SKSE\Plugins')
Copy-Item -LiteralPath (Join-Path $projectRoot 'package\SKSE\Plugins\SkyrimTextBridge.ini') -Destination (Join-Path $stage 'SKSE\Plugins')
$readme = [Text.StringBuilder]::new()
[void]$readme.Append([IO.File]::ReadAllText((Join-Path $projectRoot "release-materials\$Version\readme-intro.txt")))
$notices = [ordered]@{
    'PROJECT LICENSE — GPL-3.0-only' = 'LICENSE'
    'THIRD-PARTY NOTICES' = 'THIRD-PARTY-NOTICES.md'
    'COMMONLIBVR / COMMONLIBSSE — MIT' = 'extern\CommonLibVR\LICENSE'
    'SPDLOG — MIT' = 'extern\spdlog\LICENSE'
    'PRISMA UI — ORIGINAL LICENSE' = 'extern\PrismaUI\LICENSE.md'
    'SKSE MENU FRAMEWORK — LGPL-2.1' = 'extern\SKSEMenuFramework\LICENSE'
    'MERIDIAN UI — ORIGINAL PROJECT LICENSE (SDK HEADERS CARRY MIT SPDX NOTICES)' = 'extern\MeridianUI\LICENSE'
}
foreach ($entry in $notices.GetEnumerator()) {
    [void]$readme.Append("`r`n`r`n================================================================`r`n$($entry.Key)`r`n================================================================`r`n`r`n")
    [void]$readme.Append([IO.File]::ReadAllText((Join-Path $projectRoot $entry.Value)))
}
[IO.File]::WriteAllText((Join-Path $stage 'readme.txt'),$readme.ToString(),[Text.UTF8Encoding]::new($true))
$zip = Join-Path $dist "SkyrimTextBridge-$Version$suffix.zip"
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zip -Force
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::OpenRead($zip)
try {
    $actual = @($archive.Entries | Where-Object { $_.Name } | ForEach-Object { $_.FullName.Replace('\','/') } | Sort-Object)
    $expected = @('readme.txt','SKSE/Plugins/SkyrimTextBridge.dll','SKSE/Plugins/SkyrimTextBridge.ini') | Sort-Object
    if (Compare-Object $actual $expected) { throw 'Release ZIP must contain exactly DLL, default INI and readme.txt' }
} finally { $archive.Dispose() }
if ($Release) {
    Compress-Archive -LiteralPath (Join-Path $projectRoot 'build\native\SkyrimTextBridge.pdb') -DestinationPath (Join-Path $dist "SkyrimTextBridge-$Version-symbols.zip") -Force
}
Get-FileHash -LiteralPath $zip -Algorithm SHA256 | Format-List
