# Local changes to qBittorrent

Local-only modifications on the `my-changes` branch. `master` is an unmodified copy
of the official qBittorrent source. Nothing here is pushed to GitHub.

## How to undo a change

Run these in `D:\qbt` with qBittorrent closed, then rebuild:

```
git revert <commit>
powershell -ExecutionPolicy Bypass -File C:\qbt-deps\build-qbt.ps1
```

Revert newer changes before older ones they build on. For example, the Ban button
uses the country dropdown, so revert the Ban button first.

To see exactly what a change did: `git show <commit>`.

To get the official program back completely: `git switch master`, then rebuild.

## Changes (newest first)

### 5. Bandwidth priority column removed — commit `2364f9416` (2026-10-16)

Reverts change 3 completely on request: the "#" checkbox column and
`bandwidthprioritizer.h/.cpp` are gone, and the other files are back as before.
No torrents were checked at the time, so every torrent already had its own speed
limits. The feature's keys were removed from `qBittorrent.ini` (backup:
`qBittorrent.ini.before-priority-removal.bak`). Change 4 is unaffected.

To bring the feature back: `git revert 2364f9416`, then rebuild.

### 4. No confirmation when banning peers from the Peers tab — commit `9a12f8fce` (2026-10-15)

"Ban peer permanently" in the Peers tab (right-click menu or Delete key) bans the
selected peers right away, without the "Are you sure…?" question. The execution log
entry is unchanged. The country "Ban" button still asks first.

- Changed: `src/gui/properties/peerlistwidget.cpp`

### 3. Bandwidth priority column ("#") — commit `6621860ac` (2026-10-15) — REMOVED in change 5

A checkbox column "#" in the torrent list (tooltip: "Prioritized bandwidth").
Checked torrents get download and upload bandwidth first. Several checked torrents
share it equally; what one can't use goes to the others. Unchecked torrents share
what is left.

- New files: `src/gui/bandwidthprioritizer.h`, `src/gui/bandwidthprioritizer.cpp`
- Changed: `src/gui/CMakeLists.txt`, `mainwindow.cpp/.h`, `transferlistmodel.cpp/.h`,
  `transferlistsortmodel.cpp`, `transferlistwidget.cpp`
- Settings it stores in `%APPDATA%\qBittorrent\qBittorrent.ini`:
  - `GUI\BandwidthPriority\Torrents`: the checked torrents
  - `GUI\BandwidthPriority\OriginalLimits`: backup of the torrents' own speed limits
    while the feature is overriding them (empty when nothing is checked)
  - `GUI\BandwidthPriority\PeakDownloadRate`, `PeakUploadRate`: measured capacity
  - `TransferList\BandwidthPriorityColumnPlaced`: the column was placed first once
- **Before reverting:** uncheck all boxes and close qBittorrent normally. That restores
  every torrent's own speed limits. The keys above can then be deleted.

### 2. Ban button, list scroll bar — commit `cb7a890ca` (2026-10-15)

"Ban" button right of the country dropdown: bans all connected peers from the
selected country, with the same confirmation and execution log entry as manual
banning, plus a 5-second notification in the middle of the screen. The country
list opens with a scroll bar.

- Changed: `src/base/net/geoipmanager.cpp/.h`, `src/gui/mainwindow.cpp/.h`
- Bans are stored in qBittorrent's normal ban list. Reverting the code does not
  remove them; use Options → Connection → IP Filtering → Manually banned IP
  addresses.

### 1. Country dropdown — commit `9256e7d06` (2026-10-14)

Dropdown right of the Options button listing every country in
`dbip-country-lite.mmdb`, with flags, sorted alphabetically.

- Changed: `src/base/net/geoipdatabase.cpp/.h`, `src/base/net/geoipmanager.cpp/.h`,
  `src/gui/mainwindow.cpp/.h`
