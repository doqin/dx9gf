<#
 Assembles a fully portable folder + zip of the demo (no installs needed on the user's PC).
 Bundles: demo.exe, tmxlite.dll, d3dx9_43.dll (x64), app-local VC++ runtime DLLs, assets.
 Run after building demo.vcxproj as Release|x64.
#>
param(
    [string]$OutDir  = "dist/DX9GF-Demo",
    [string]$DxSdk   = "${env:ProgramFiles(x86)}\Microsoft DirectX SDK (June 2010)"
)
$ErrorActionPreference = "Stop"
$root = Resolve-Path (Join-Path $PSScriptRoot "..")
Set-Location $root

$exe = Get-ChildItem -Recurse -Filter demo.exe |
       Where-Object { $_.FullName -match "x64.Release" } | Select-Object -First 1
if (-not $exe) { throw "demo.exe (Release x64) not found - build first." }

Remove-Item $OutDir -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $OutDir | Out-Null

# 1. game exe, tmxlite, assets (game loads "assets/..." relative to the exe folder)
Copy-Item $exe.FullName $OutDir
Copy-Item external/tmxlite.dll $OutDir
Copy-Item demo/assets (Join-Path $OutDir "assets") -Recurse

# 2. d3dx9_43.dll (x64) from the DirectX SDK redist cab
$tmp = Join-Path $env:TEMP "dx9cab"; New-Item -ItemType Directory -Force $tmp | Out-Null
expand.exe "$DxSdk\Redist\Jun2010_d3dx9_43_x64.cab" -F:d3dx9_43.dll $tmp | Out-Null
Copy-Item "$tmp\d3dx9_43.dll" $OutDir

# 3. app-local VC++ runtime (msvcp140, vcruntime140, vcruntime140_1, ...)
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products * -property installationPath
$crt = Get-ChildItem "$vs\VC\Redist\MSVC\*\x64\Microsoft.VC14*.CRT" -Directory |
       Sort-Object FullName | Select-Object -Last 1
if (-not $crt) { throw "VC++ redist CRT folder not found." }
Copy-Item "$($crt.FullName)\*.dll" $OutDir

# 4. zip
$zip = "$OutDir.zip"
Remove-Item $zip -ErrorAction SilentlyContinue
Compress-Archive -Path "$OutDir\*" -DestinationPath $zip
Write-Host "Portable build: $zip"
