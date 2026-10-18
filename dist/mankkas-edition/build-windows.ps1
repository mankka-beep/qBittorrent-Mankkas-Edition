# Builds qBittorrent Mankka's Edition on Windows (x64), the same way qBittorrent's own
# Windows CI does. See docs/mankkas-edition/BUILDING.md for the tools it needs.
#
#   powershell -ExecutionPolicy Bypass -File dist\mankkas-edition\build-windows.ps1 [options]
#
# Options (all optional):
#   -DepsDir <folder>   where Boost and libtorrent are downloaded and built (default C:\qbt-deps)
#   -VcpkgDir <folder>  the vcpkg folder (default C:\vcpkg)
#   -QtDir <folder>     the Qt 6 MSVC folder (default C:\Qt\6.10.3\msvc2022_64)
#   -Clean              delete the previous build folder first
#   -SkipTests          don't build and run the unit tests
#
# Each dependency step is skipped when its result already exists. The finished program is
# build\qbittorrent.exe in the source folder, ready to run in place.
param(
    [string]$DepsDir = 'C:\qbt-deps',
    [string]$VcpkgDir = 'C:\vcpkg',
    [string]$QtDir = 'C:\Qt\6.10.3\msvc2022_64',
    [switch]$Clean,
    [switch]$SkipTests
)
$ErrorActionPreference = 'Stop'

$boostVersion = '1.91.0'
$libtorrentVersion = '2.0.15'

$src        = (Resolve-Path "$PSScriptRoot\..\..").Path
$boost      = "$DepsDir\boost"
$libtorrent = "$DepsDir\libtorrent"
$triplet    = 'x64-windows-static-md-release'

function Check { if ($LASTEXITCODE -ne 0) { throw "Step failed with exit code $LASTEXITCODE" } }

foreach ($required in @("$VcpkgDir\vcpkg.exe", "$QtDir\bin\windeployqt.exe")) {
    if (-not (Test-Path $required)) { throw "Not found: $required (see docs\mankkas-edition\BUILDING.md)" }
}

# MSVC developer environment (x64)
$vs = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw 'Visual Studio 2022 (Build Tools) with the C++ workload was not found' }
& "$vs\Common7\Tools\Launch-VsDevShell.ps1" -Arch amd64 -HostArch amd64 -SkipAutomaticLocation | Out-Null
$env:Path = "$QtDir\bin;" + [Environment]::GetEnvironmentVariable('Path','Machine') + ';' + [Environment]::GetEnvironmentVariable('Path','User') + ";$env:Path"
$env:VCPKG_DISABLE_METRICS = '1'
New-Item -ItemType Directory -Force $DepsDir | Out-Null

# 1. vcpkg: OpenSSL, zlib and b2 (Boost.Build), static libraries with the dynamic C runtime
$overlay = "$VcpkgDir\triplets_overlay"
New-Item -ItemType Directory -Force $overlay | Out-Null
Set-Content -Encoding ascii "$overlay\$triplet.cmake" @(
    'set(VCPKG_TARGET_ARCHITECTURE x64)',
    'set(VCPKG_LIBRARY_LINKAGE static)',
    'set(VCPKG_CRT_LINKAGE dynamic)',
    'set(VCPKG_BUILD_TYPE release)')
& "$VcpkgDir\vcpkg.exe" install --clean-after-build --overlay-triplets="$overlay" "boost-build:$triplet" "openssl:$triplet" "zlib:$triplet"; Check

# 2. Boost headers + CMake config
if (-not (Test-Path $boost)) {
    $boostFolder = 'boost_' + $boostVersion.Replace('.', '_')
    $archive = "$DepsDir\$boostFolder.tar.gz"
    curl.exe -sSL -f -o $archive "https://archives.boost.io/release/$boostVersion/source/$boostFolder.tar.gz"; Check
    tar -xf $archive -C $DepsDir; Check
    Rename-Item "$DepsDir\$boostFolder" 'boost'
    Remove-Item $archive
}
if (-not (Test-Path "$boost\lib\cmake")) {
    Push-Location $boost
    & "$VcpkgDir\installed\$triplet\tools\boost-build\b2.exe" stage toolset=msvc --stagedir=.\ --with-headers; Check
    Pop-Location
}

# 3. libtorrent (static)
if (-not (Test-Path $libtorrent)) {
    git clone --quiet --branch "v$libtorrentVersion" --depth 1 --recurse-submodules https://github.com/arvidn/libtorrent.git $libtorrent; Check
}
if (-not (Test-Path "$libtorrent\install\lib\cmake\LibtorrentRasterbar")) {
    $saved = $env:CXXFLAGS, $env:CFLAGS, $env:LDFLAGS
    $env:CFLAGS = "$env:CFLAGS /guard:cf"; $env:CXXFLAGS = "$env:CXXFLAGS /guard:cf"; $env:LDFLAGS = "$env:LDFLAGS /GUARD:CF"
    cmake -S $libtorrent -B "$libtorrent\build" -G Ninja `
        -DCMAKE_BUILD_TYPE=RelWithDebInfo `
        -DCMAKE_CXX_STANDARD=20 `
        "-DCMAKE_INSTALL_PREFIX=$libtorrent\install" `
        "-DCMAKE_TOOLCHAIN_FILE=$VcpkgDir\scripts\buildsystems\vcpkg.cmake" `
        "-DBOOST_ROOT=$boost\lib\cmake" `
        -DBUILD_SHARED_LIBS=OFF `
        -Ddeprecated-functions=OFF `
        -Dstatic_runtime=OFF `
        "-DVCPKG_TARGET_TRIPLET=$triplet"; Check
    cmake --build "$libtorrent\build"; Check
    cmake --install "$libtorrent\build"; Check
    $env:CXXFLAGS, $env:CFLAGS, $env:LDFLAGS = $saved
}

# 4. qBittorrent Mankka's Edition
$build = "$src\build"
if ($Clean -and (Test-Path $build)) { Remove-Item $build -Recurse -Force }
$testing = if ($SkipTests) { 'OFF' } else { 'ON' }
cmake -S $src -B $build -G Ninja `
    -DCMAKE_BUILD_TYPE=RelWithDebInfo `
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON `
    "-DCMAKE_PREFIX_PATH=$QtDir" `
    "-DCMAKE_TOOLCHAIN_FILE=$VcpkgDir\scripts\buildsystems\vcpkg.cmake" `
    "-DBOOST_ROOT=$boost\lib\cmake" `
    "-DLibtorrentRasterbar_DIR=$libtorrent\install\lib\cmake\LibtorrentRasterbar" `
    -DMSVC_RUNTIME_DYNAMIC=ON `
    "-DTESTING=$testing" `
    "-DVCPKG_TARGET_TRIPLET=$triplet"; Check
cmake --build $build; Check
if (-not $SkipTests) {
    # Report failing unit tests but keep going: testutilsmisc assumes a "." decimal
    # separator and fails under locales such as Finnish that use ",".
    cmake --build $build --target check
    if ($LASTEXITCODE -ne 0) { Write-Warning 'Some unit tests failed; see the ctest output above.' }
}

# 5. Make build\qbittorrent.exe runnable in place: Qt DLLs + plugins next to it
& "$QtDir\bin\windeployqt.exe" --no-translations --no-compiler-runtime --no-system-d3d-compiler --no-opengl-sw --plugindir "$build\plugins" "$build\qbittorrent.exe"; Check
Copy-Item "$src\dist\windows\qt.conf" $build -Force
Write-Host "`nBuilt: $build\qbittorrent.exe" -ForegroundColor Green
