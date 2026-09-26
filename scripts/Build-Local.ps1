param(
    [string]$CommonLibArchive = '',
    [string]$SpdlogSource = ''
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
Set-Location -LiteralPath $projectRoot
& node tests/MeridianTests.cjs
if ($LASTEXITCODE) { throw 'Meridian DOM checks failed' }
$meridianScript = Get-Content -LiteralPath 'assets/MeridianBridge.js' -Raw
('#pragma once' + "`n" + 'inline constexpr char MeridianScript[] = R"STBJS(' + $meridianScript + ')STBJS";' + "`n") | Set-Content -Encoding utf8 'include/MeridianBridge.generated.h'
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vs) { throw 'Visual Studio C++ tools not found' }
$msvc = Get-ChildItem -LiteralPath (Join-Path $vs 'VC\Tools\MSVC') -Directory | Sort-Object Name -Descending | Select-Object -First 1
$sdkRoot = (Get-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows Kits\Installed Roots').KitsRoot10
$sdkVersion = (Get-ChildItem -LiteralPath (Join-Path $sdkRoot 'Include') -Directory | Sort-Object Name -Descending | Select-Object -First 1).Name
$compilerDir = Join-Path $msvc.FullName 'bin\Hostx64\x64'
$env:PATH = "$compilerDir;$env:PATH"
$env:INCLUDE = @((Join-Path $msvc.FullName 'include'),(Join-Path $sdkRoot "Include\$sdkVersion\ucrt"),(Join-Path $sdkRoot "Include\$sdkVersion\shared"),(Join-Path $sdkRoot "Include\$sdkVersion\um"),(Join-Path $sdkRoot "Include\$sdkVersion\winrt")) -join ';'
$env:LIB = @((Join-Path $msvc.FullName 'lib\x64'),(Join-Path $sdkRoot "Lib\$sdkVersion\ucrt\x64"),(Join-Path $sdkRoot "Lib\$sdkVersion\um\x64")) -join ';'
if (!$CommonLibArchive) { $CommonLibArchive = Join-Path $projectRoot 'extern\CommonLibVR\lib\commonlibsse-ng.lib' }
if (!$SpdlogSource -and (Test-Path -LiteralPath (Join-Path $projectRoot 'extern\spdlog\src\spdlog.cpp'))) { $SpdlogSource = Join-Path $projectRoot 'extern\spdlog' }
if (!$SpdlogSource) {
    $candidate = Get-ChildItem -Path (Join-Path $projectRoot 'tools-cache\xmake\.xmake\cache\packages\*\s\spdlog\v1.16.0\source\spdlog') -Directory | Select-Object -First 1
    if ($candidate) { $SpdlogSource = $candidate.FullName }
}
if (!(Test-Path -LiteralPath $CommonLibArchive)) { throw 'Supply an AE+SE, non-VR, MSVC /MD CommonLibVR library matching extern headers.' }
if (!(Test-Path -LiteralPath (Join-Path $SpdlogSource 'include\spdlog\spdlog.h'))) { throw 'Supply spdlog 1.16.0 source directory.' }
$outputDir = Join-Path $projectRoot 'build\native'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$common = @('/nologo','/std:c++latest','/utf-8','/EHsc','/MD','/O2','/Zi','/FS','/permissive-','/Zc:__cplusplus','/Zc:preprocessor','/DNOMINMAX','/DWIN32_LEAN_AND_MEAN','/DUNICODE','/D_UNICODE',"/I$projectRoot\include")
& cl @common tests/CoreTests.cpp "/Fe$outputDir\BridgeCoreTests.exe" "/Fo$outputDir\CoreTests.obj" /link /INCREMENTAL:NO
if ($LASTEXITCODE) { throw 'Core test compilation failed' }
& "$outputDir\BridgeCoreTests.exe"
if ($LASTEXITCODE) { throw 'Core tests failed' }
& cl @common tests/DirectImeHarness.cpp "/Fe$outputDir\DirectImeHarness.exe" "/Fo$outputDir\DirectImeHarness.obj" /link user32.lib imm32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib /INCREMENTAL:NO
if ($LASTEXITCODE) { throw 'Direct IME harness compilation failed' }
& "$outputDir\DirectImeHarness.exe"
if ($LASTEXITCODE) { throw 'Direct IME lifecycle tests failed' }
& cl @common tests/InputMethodNameTests.cpp "/Fe$outputDir\InputMethodNameTests.exe" "/Fo$outputDir\InputMethodNameTests.obj" /link user32.lib imm32.lib ole32.lib oleaut32.lib uuid.lib /INCREMENTAL:NO
if ($LASTEXITCODE) { throw 'IME name test compilation failed' }
& "$outputDir\InputMethodNameTests.exe"
if ($LASTEXITCODE) { throw 'IME name tests failed' }
& cl @common "/I$projectRoot\extern\PrismaUI" tests/PrismaBridgeTests.cpp "/Fe$outputDir\PrismaBridgeTests.exe" "/Fo$outputDir\PrismaBridgeTests.obj" /link /INCREMENTAL:NO
if ($LASTEXITCODE) { throw 'Prisma negotiation test compilation failed' }
& "$outputDir\PrismaBridgeTests.exe"
if ($LASTEXITCODE) { throw 'Prisma negotiation tests failed' }
New-Item -ItemType Directory -Path (Join-Path $projectRoot 'build\preview') -Force | Out-Null
& cl @common tools/SettingsChecks.cpp "/Fe$outputDir\SettingsChecks.exe" "/Fo$outputDir\SettingsChecks.obj" /link user32.lib gdi32.lib /INCREMENTAL:NO
if ($LASTEXITCODE) { throw 'Settings checks compilation failed' }
& "$outputDir\SettingsChecks.exe"
if ($LASTEXITCODE) { throw 'Settings and alpha checks failed' }
& cl @common tools/MeridianAbiCheck.cpp "/Fe$outputDir\MeridianAbiCheck.exe" "/Fo$outputDir\MeridianAbiCheck.obj" /link /INCREMENTAL:NO
if ($LASTEXITCODE) { throw 'Meridian ABI check compilation failed' }
& "$outputDir\MeridianAbiCheck.exe"
if ($LASTEXITCODE) { throw 'Meridian ABI check failed' }
$pluginFlags = @('/DENABLE_SKYRIM_SE=1','/DENABLE_SKYRIM_AE=1','/DTEXTBRIDGE_MANUAL_BUILD','/DSPDLOG_COMPILED_LIB','/DSPDLOG_USE_STD_FORMAT','/DSPDLOG_WCHAR_TO_UTF8_SUPPORT',"/I$projectRoot\extern\CommonLibVR\include", "/I$projectRoot\extern\PrismaUI", "/I$SpdlogSource\include")
$sources = @((Get-ChildItem -LiteralPath (Join-Path $projectRoot 'src') -Filter '*.cpp' | Where-Object Name -NE 'Composer.cpp').FullName)
$sources += @('spdlog.cpp','stdout_sinks.cpp','color_sinks.cpp','file_sinks.cpp','async.cpp','cfg.cpp') | ForEach-Object { Join-Path $SpdlogSource "src\$_" }
& cl @common @pluginFlags /c /MP @sources "/Fo$outputDir\\"
if ($LASTEXITCODE) { throw 'Plugin compilation failed' }
$objects = @($sources | ForEach-Object { Join-Path $outputDir ([IO.Path]::GetFileNameWithoutExtension($_) + '.obj') })
& link /NOLOGO /DLL /DEBUG /INCREMENTAL:NO "/OUT:$outputDir\SkyrimTextBridge.dll" @objects $CommonLibArchive user32.lib gdi32.lib imm32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib shell32.lib advapi32.lib version.lib d3d11.lib dxgi.lib d3dcompiler.lib dbghelp.lib bcrypt.lib
if ($LASTEXITCODE) { throw 'Plugin link failed' }
& cl @common tools/RuntimeMetadataCheck.cpp "/Fe$outputDir\RuntimeMetadataCheck.exe" "/Fo$outputDir\RuntimeMetadataCheck.obj" /link user32.lib /INCREMENTAL:NO
if ($LASTEXITCODE) { throw 'Metadata checker compilation failed' }
& "$outputDir\RuntimeMetadataCheck.exe" "$outputDir\SkyrimTextBridge.dll"
if ($LASTEXITCODE) { throw 'SE/AE plugin metadata check failed' }
Write-Output "Built $outputDir\SkyrimTextBridge.dll"
