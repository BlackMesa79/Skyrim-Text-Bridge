$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$source = Get-Content -LiteralPath (Join-Path $projectRoot 'assets\MeridianBridge.js') -Raw
if ($source.Contains(')STBJS"')) { throw 'Raw string delimiter collision' }
$header = '#pragma once' + "`n" + 'inline constexpr char MeridianScript[] = R"STBJS(' + $source + ')STBJS";' + "`n"
Set-Content -LiteralPath (Join-Path $projectRoot 'include\MeridianBridge.generated.h') -Value $header -Encoding utf8
