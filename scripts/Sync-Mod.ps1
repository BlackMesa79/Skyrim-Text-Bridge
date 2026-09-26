param(
    [string]$Version = '0.3.7',
    [switch]$Release,
    [string]$Target = 'H:\Games\Dev Skyrim\mods\36 - SkyrimTextBridge'
)
$ErrorActionPreference = 'Stop'
if (Get-Process SkyrimSE -ErrorAction SilentlyContinue) { throw 'Exit Skyrim before syncing its loaded DLL.' }
$project = Split-Path $PSScriptRoot -Parent
$targetRoot = (Resolve-Path -LiteralPath $Target).Path.TrimEnd('\')
$suffix = if ($Release) { '' } else { '-prototype' }
$source = Join-Path $project ("build\deploy-$Version$suffix-release-sync")
Expand-Archive -LiteralPath (Join-Path $project "dist\SkyrimTextBridge-$Version$suffix.zip") -DestinationPath $source -Force
$backup = Join-Path $project ('build\deployment-backup-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$files = Get-ChildItem -LiteralPath $source -Recurse -File
# Back up every replaced file before changing any destination file. Never delete extras or MO2 metadata.
foreach ($file in $files) {
    $relative = $file.FullName.Substring($source.Length + 1)
    $destination = [IO.Path]::GetFullPath((Join-Path $targetRoot $relative))
    if (!$destination.StartsWith($targetRoot + '\', [StringComparison]::OrdinalIgnoreCase)) { throw 'Invalid destination' }
    if (Test-Path -LiteralPath $destination) {
        $backupFile = Join-Path $backup $relative
        New-Item -ItemType Directory -Path (Split-Path $backupFile -Parent) -Force | Out-Null
        Copy-Item -LiteralPath $destination -Destination $backupFile
    }
}
foreach ($file in $files) {
    $destination = Join-Path $targetRoot $file.FullName.Substring($source.Length + 1)
    New-Item -ItemType Directory -Path (Split-Path $destination -Parent) -Force | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
    if ((Get-FileHash -LiteralPath $file.FullName).Hash -ne (Get-FileHash -LiteralPath $destination).Hash) { throw "Hash mismatch: $destination" }
}
Write-Output "Synced $Version; verified $($files.Count) files. Backup: $backup"
