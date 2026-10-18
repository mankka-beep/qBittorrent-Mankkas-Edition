# Packs the built program (build\qbittorrent.exe, see build-windows.ps1) into the zip file
# that is published as a release: qBittorrent-Mankkas-Edition-vX.Y.Z-win64.zip
#
#   powershell -ExecutionPolicy Bypass -File dist\mankkas-edition\package-release.ps1 [-OutputDir <folder>]
#
# The zip contains the program with the Qt DLLs and plugins it needs, the Microsoft Visual C++
# runtime DLLs (so it also runs where the Visual C++ Redistributable isn't installed), the
# licences, the README, the user guide and the change log.
param(
    [string]$OutputDir,
    # Qt folder used for the build; its translations\qtbase_*.qm are added to the zip
    [string]$QtDir = 'C:\Qt\6.10.3\msvc2022_64'
)
$ErrorActionPreference = 'Stop'

$src = (Resolve-Path "$PSScriptRoot\..\..").Path
$build = "$src\build"
if (-not $OutputDir) { $OutputDir = "$src\release" }

if (-not (Test-Path "$build\qbittorrent.exe")) { throw "Build the program first: $build\qbittorrent.exe not found" }

# Version of the edition, e.g. v1.1.0
$versionLine = Select-String -Path "$src\src\base\version.h.in" -Pattern '#define QBT_MOD_VERSION "(v[0-9.]+)"'
if (-not $versionLine) { throw 'QBT_MOD_VERSION not found in src\base\version.h.in' }
$version = $versionLine.Matches[0].Groups[1].Value

$name = "qBittorrent-Mankkas-Edition-$version-win64"
$stage = Join-Path $OutputDir $name
$zip = "$stage.zip"
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
if (Test-Path $zip) { Remove-Item $zip -Force }
New-Item -ItemType Directory -Force $stage | Out-Null

# Program, Qt and the Qt plugins qBittorrent uses (the same set as the official qBittorrent
# package, plus network information). Not copied: dxcompiler.dll and dxil.dll (only needed
# for 3D graphics), icuuc.dll (part of Windows itself) and unused plugins.
Copy-Item "$build\qbittorrent.exe", "$build\qt.conf" $stage
Get-ChildItem $build -Filter 'Qt6*.dll' | Copy-Item -Destination $stage
$plugins = @(
    'iconengines\qsvgicon.dll',
    'imageformats\*.dll',
    'networkinformation\*.dll',
    'platforms\qwindows.dll',
    'sqldrivers\qsqlite.dll',
    'styles\qmodernwindowsstyle.dll',
    'tls\*.dll'
)
foreach ($plugin in $plugins) {
    $targetFolder = Join-Path "$stage\plugins" (Split-Path $plugin -Parent)
    New-Item -ItemType Directory -Force $targetFolder | Out-Null
    $files = Get-ChildItem (Join-Path "$build\plugins" $plugin)
    if (-not $files) { throw "Qt plugin not found: $build\plugins\$plugin" }
    $files | Copy-Item -Destination $targetFolder
}

# Qt's own translations (buttons and standard dialogs in the user's language), loaded from
# the "translations" folder named in qt.conf
$qtTranslations = Get-ChildItem "$QtDir\translations" -Filter 'qtbase_*.qm' -ErrorAction SilentlyContinue
if (-not $qtTranslations) { throw "Qt translations not found in $QtDir\translations (install the qttranslations archive, see BUILDING.md)" }
New-Item -ItemType Directory -Force "$stage\translations" | Out-Null
$qtTranslations | Copy-Item -Destination "$stage\translations"

# Microsoft Visual C++ runtime DLLs (redistributable)
$vs = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$crt = Get-ChildItem "$vs\VC\Redist\MSVC\*\x64\Microsoft.VC14*.CRT" -Directory | Sort-Object FullName | Select-Object -Last 1
if (-not $crt) { throw 'Visual C++ runtime DLLs (VC\Redist\MSVC\...\x64\Microsoft.VC14*.CRT) not found' }
Get-ChildItem $crt.FullName -Filter '*.dll' | Copy-Item -Destination $stage

# Licences and documents
New-Item -ItemType Directory -Force "$stage\licenses" | Out-Null
Copy-Item "$src\dist\mankkas-edition\licenses\*" "$stage\licenses"
Copy-Item "$src\COPYING", "$src\COPYING.GPLv2", "$src\COPYING.GPLv3", "$src\AUTHORS" "$stage\licenses"
Copy-Item "$src\README.md", "$src\LOCAL_CHANGES.md", "$src\docs\mankkas-edition\USER_GUIDE.md" $stage

# Written entry by entry so that folders are separated with the standard "/" (Windows
# PowerShell 5's zip functions write "\", which some unzip programs don't understand)
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::Open($zip, [IO.Compression.ZipArchiveMode]::Create)
try {
    foreach ($file in (Get-ChildItem $stage -Recurse -File)) {
        $entryName = $file.FullName.Substring($stage.Length + 1).Replace('\', '/')
        [void][IO.Compression.ZipFileExtensions]::CreateEntryFromFile($archive, $file.FullName, $entryName, [IO.Compression.CompressionLevel]::Optimal)
    }
}
finally {
    $archive.Dispose()
}
$size = '{0:N1} MB' -f ((Get-Item $zip).Length / 1MB)
Write-Host "Created $zip ($size)" -ForegroundColor Green
