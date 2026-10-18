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

### 15. Ban box in the lower left corner, flags in the Blocked IPs tab — commit `690d84d70` (2026-10-18)

- The 5-second result box after "Ban" / "Auto Ban" now appears in the lower left
  corner of the screen (above the taskbar) instead of the middle. The "Total bans"
  window is unchanged.
- In Execution Log → Blocked IPs, each row shows the IP's country flag right after
  the IP address, sized to the row height. The country comes from the same GeoIP
  database as the Peers tab, so flags only appear while "Resolve peer countries" is on.
  Copying rows (Ctrl+C) still copies plain text.
- Changed: `src/gui/mainwindow.cpp/.h`, `src/gui/log/logmodel.cpp/.h`,
  `src/gui/log/loglistview.cpp`
- Undo with `git revert 690d84d70`.

### 14. "Select All" toolbar button — commit `7d1cab5af` (2026-10-17)

A toolbar button left of the country dropdown, in the same style and size as the other
toolbar buttons. It selects all torrents in the list, the same as Ctrl+A, switching to
the Transfers tab first if another tab is open. Its icon is new: three highlighted list
rows with ticked checkboxes, in qBittorrent's usual blue.

- New file: `src/icons/select-all.svg`
- Changed: `src/icons/icons.qrc`, `src/gui/uithemecommon.h`, `src/gui/mainwindow.cpp`
- Undo with `git revert 7d1cab5af`.

### 13. No confirmation when pressing "Ban" — commit `49199e487` (2026-10-17)

Pressing "Ban" bans the peers from the checked countries right away, without the
"Are you sure…?" question, the same as Auto Ban. The log entries and the result box
with "Today:" are unchanged. No ban anywhere in the program asks for confirmation now.

- Changed: `src/gui/mainwindow.cpp/.h`
- Undo with `git revert 49199e487`.

### 12. "Auto Ban" button — commit `fb363d923` (2026-10-17)

An "Auto Ban" toggle button right of "Ban". Red = off, green = on; each press switches.
It always starts red (off) when the program starts. While it's on, the ban routine
runs right away and then once a minute, exactly like pressing "Ban" (same log entries,
same result box with "Today:"), except that it doesn't ask "Are you sure…?". Turning
it on is the confirmation. A run is skipped if no country is checked or the previous
ban is still collecting peers. The "already banned?" check now uses a set, so it stays
fast with 10,000+ bans.

- Changed: `src/gui/mainwindow.cpp/.h`
- No settings stored.
- Undo with `git revert fb363d923`.

### 11. "Total bans" button and window — commit `86cc8e87c` (2026-10-17)

A "Total bans" button left of the "Filter torrents..." box opens a window listing
qBittorrent's manually banned IP addresses per country (flag, country, number of
bans), sorted from the most bans to the fewest, with the total below and a Close
button. Countries come from `dbip-country-lite.mmdb`; IPs it can't place are
counted under "N/A". The button is in the same part of the toolbar as the filter
box, so it's only shown on the Transfers tab. The window remembers its size.

- New files: `src/gui/totalbansdialog.h`, `src/gui/totalbansdialog.cpp`
- Changed: `src/gui/CMakeLists.txt`, `src/gui/mainwindow.cpp`
- Setting in `qBittorrent.ini`: `TotalBansDialog\Size` (window size). Safe to delete
  after undoing.
- Undo with `git revert 86cc8e87c`.

### 10. "Today:" ban counter in the ban notification — commit `455864bc6` (2026-10-17)

The notification after pressing "Ban" has a solid line at the bottom and below it
"Today: N", the number of peers banned today: with the Ban button plus peers banned
by hand in the Peers tab. Each IP counts once, and IPs that were already banned don't
count. The counter is saved, so it survives restarts and rebuilds, and starts again
from 0 when the date changes.

- New files: `src/gui/bancounter.h`, `src/gui/bancounter.cpp`
- Changed: `src/gui/CMakeLists.txt`, `src/gui/mainwindow.cpp/.h`,
  `src/gui/properties/peerlistwidget.cpp`
- Settings in `qBittorrent.ini` under `[GUI]`: `BanCountries\TodayDate` (the date the
  count belongs to) and `BanCountries\TodayCount`. Safe to delete after undoing.
- Undo with `git revert 455864bc6`.

### 9. Checked countries are remembered — commit `cf2fb468d` (2026-10-16)

The countries checked in the country dropdown stay checked after closing and
restarting qBittorrent, and after rebuilding it. They are saved every time a
country is checked or unchecked. Ban is enabled at startup if any are checked.

- Changed: `src/gui/countrycombobox.cpp/.h`, `src/gui/mainwindow.cpp/.h`
- Settings key: `GUI\BanCountries\Checked` (list of country codes)
- Undo with `git revert cf2fb468d`; the key can then be deleted.

### 8. Idle force-started seeds keep blue arrows — commit `6f1ca312f` (2026-10-16)

Fix for change 7: a force-started seeding torrent counts as "uploading" all the
time, so it showed green arrows even when idle. Now it shows green only while
something is actually being uploaded, otherwise the light blue idle arrows.

- Changed: `src/gui/transferlistmodel.cpp`
- Undo with `git revert 6f1ca312f` (revert before change 7).

### 7. Green arrows for actively uploading torrents — commit `80a680878` (2026-10-16)

In the torrent list, torrents that are uploading right now show green up-arrows
instead of blue, the same green as downloading torrents. Idle seeding torrents
(nobody downloading from you) keep their light blue arrows. The upload speed in
the status bar and the "Seeding" filter keep the original blue icon.

- New file: `src/icons/uploading.svg` (copy of `upload.svg`, colour `#008000`)
- Changed: `src/icons/icons.qrc`, `src/gui/uithemecommon.h`, `src/gui/transferlistmodel.cpp`
- Undo with `git revert 80a680878`.

### 6. Check several countries, ban them all at once — commit `a177a36bb` (2026-10-16)

The country dropdown has a checkbox left of each flag. Clicking a country (or pressing
Space) checks or unchecks it and keeps the list open. The closed dropdown shows
"Select countries", the one checked country, or "N countries", and its tooltip lists
them. "Ban" bans connected peers from all checked countries in one go (one
confirmation, same log entry per IP); the notification shows the total, plus a line
per country when several are checked. Ban is greyed out until a country is checked.
Checked countries were not remembered after a restart (fixed by change 9).

- New files: `src/gui/countrycombobox.h`, `src/gui/countrycombobox.cpp` (the list's
  scroll-bar style moved here from `mainwindow.cpp`)
- Changed: `src/gui/CMakeLists.txt`, `src/gui/mainwindow.cpp/.h`
- Undo with `git revert a177a36bb`: the dropdown goes back to choosing one country.

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
