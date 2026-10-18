# Building qBittorrent Mankka's Edition on Windows

This builds the 64-bit Windows program from source, the same way qBittorrent's own
Windows builds are made. It needs about 15 GB of disk space, and the first build takes
roughly an hour (later builds take a few minutes).

Most people don't need this — the ready-made download is on the **Releases** page.

## 1. Install the tools (once)

Run these in **PowerShell**. Windows will ask for administrator permission for some of them.

```powershell
# Compiler, build tools, Git and Python
winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows11SDK.26100"
winget install --id Kitware.CMake -e
winget install --id Ninja-build.Ninja -e
winget install --id Git.Git -e
winget install --id Python.Python.3.13 -e
```

Close and reopen PowerShell, then:

```powershell
# vcpkg (provides OpenSSL and zlib)
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat -disableMetrics

# Qt 6.10.3 for MSVC 2022, 64-bit (via aqtinstall)
python -m venv C:\qbt-deps\aqt-venv
C:\qbt-deps\aqt-venv\Scripts\python.exe -m pip install aqtinstall
C:\qbt-deps\aqt-venv\Scripts\aqt.exe install-qt windows desktop 6.10.3 win64_msvc2022_64 --archives qtbase qtsvg qttools -m qtimageformats -O C:\Qt
```

Boost 1.91 and libtorrent 2.0.15 are downloaded automatically by the build script.

## 2. Get the source

```powershell
git clone <this repository's URL> C:\src\qbt-mankka
cd C:\src\qbt-mankka
```

## 3. Build

```powershell
powershell -ExecutionPolicy Bypass -File dist\mankkas-edition\build-windows.ps1
```

The program is `build\qbittorrent.exe`, ready to run from that folder.

Options:

| Option | Meaning | Default |
|---|---|---|
| `-DepsDir <folder>` | where Boost and libtorrent are downloaded and built | `C:\qbt-deps` |
| `-VcpkgDir <folder>` | the vcpkg folder | `C:\vcpkg` |
| `-QtDir <folder>` | the Qt MSVC folder | `C:\Qt\6.10.3\msvc2022_64` |
| `-Clean` | delete the previous build first | |
| `-SkipTests` | don't build and run the unit tests | |

One unit test (`testutilsmisc`) fails on computers that use a decimal comma (e.g.
Finnish, German or Swedish regional settings). That's expected and doesn't affect the
program.

## 4. Make the release zip (optional)

```powershell
powershell -ExecutionPolicy Bypass -File dist\mankkas-edition\package-release.ps1
```

This creates `release\qBittorrent-Mankkas-Edition-vX.Y.Z-win64.zip` with the program, the
Qt and Visual C++ runtime DLLs, the licences, the README, the user guide and the change
log. The version number is taken from `QBT_MOD_VERSION` in `src\base\version.h.in`.
