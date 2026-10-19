# Local changes to qBittorrent (Mankka's Edition)

Local-only modifications on the `my-changes` branch. `master` is an unmodified copy
of the official qBittorrent source. Nothing here is pushed to GitHub.

On 2026-10-18 the author of all commits was changed to "Mankka" with the GitHub private
address `340949780+mankka-beep@users.noreply.github.com` (the personal email was removed
from the repository). That gave every commit a new ID; the IDs below are the new ones. (The
text of the revert commit for change 5 still quotes the old ID of change 3.)

## Version numbers

The edition is called "qBittorrent vX.Y.Z Mankka's Edition" (`QBT_MOD_VERSION` in
`src/base/version.h.in`). The version goes up with every change:

- **Y + 1** (Z back to 0): a new feature or a visible change
- **Z + 1**: a fix or a small tweak of an existing feature
- **X + 1** (Y and Z back to 0): moving to a newer official qBittorrent release, or a
  big rework

Changes that only affect the repository or the documents, not the program itself, don't
change the version.

| Version | Changes | Based on qBittorrent |
|---|---|---|
| v1.6.3 | 36 | v5.4.0alpha1 |
| v1.6.2 | 35 | v5.4.0alpha1 |
| v1.6.1 | 34 | v5.4.0alpha1 |
| v1.6.0 | 33 | v5.4.0alpha1 |
| v1.5.0 | 32 | v5.4.0alpha1 |
| v1.4.0 | 31 | v5.4.0alpha1 |
| v1.3.0 | 30 | v5.4.0alpha1 |
| v1.2.2 | 29 | v5.4.0alpha1 |
| v1.2.1 | 28 | v5.4.0alpha1 |
| v1.2.0 | 27 | v5.4.0alpha1 |
| v1.1.2 | 26 | v5.4.0alpha1 |
| v1.1.1 | 24–25 (25: packaging only) | v5.4.0alpha1 |
| v1.1.0 | 21–23 (23: repository and release preparation only) | v5.4.0alpha1 |
| v1.0.0 | 1–20 (first named version) | v5.4.0alpha1 |

## How to undo a change

Run these in `D:\qbt` with qBittorrent closed, then rebuild:

```
git revert <commit>
powershell -ExecutionPolicy Bypass -File dist\mankkas-edition\build-windows.ps1
```

Revert newer changes before older ones they build on. For example, the Ban button
uses the country dropdown, so revert the Ban button first.

To see exactly what a change did: `git show <commit>`.

To get the official program back completely: `git switch master`, then rebuild.

## Changes (newest first)

### 36. White tick box for the mute checkbox, "Total Bans" button text, version v1.6.3 — commit `75475c59c` (2026-10-19)

The mute checkbox inside the Auto Ban button now has a white tick box with a dark border
and a dark tick (new icon `checkbox-checkmark.svg`), so it shows clearly on the red and
green button. The button left of "Filter torrents..." now reads "Total Bans" (the window it
opens is still titled "Total bans"). Checked in the running program.

- Changed: `src/gui/mainwindow.cpp`, `src/icons/checkbox-checkmark.svg` (new),
  `src/icons/icons.qrc`, `src/base/version.h.in` (v1.6.3),
  `docs/mankkas-edition/USER_GUIDE.md` (version and button name)
- Undo with `git revert 75475c59c`.

### 35. Mute checkbox no longer overlaps "Auto Ban", version v1.6.2 — commit `e6a371c14` (2026-10-19)

Fix for change 34: the space reserved for the mute checkbox was measured before its speaker
icon was set, so it was too small and the checkbox and speaker were drawn over the text.
The width is now measured after the icon is set, and the checkbox is sized to its actual
parts (tick box, spacing, 16 px icon) instead of its size hint, which included unused space.
Checked in the running program: "Auto Ban", checkbox and speaker side by side, button no
wider than needed.

- Changed: `src/gui/mainwindow.cpp`, `src/base/version.h.in` (v1.6.2),
  `docs/mankkas-edition/USER_GUIDE.md` (version only)
- Undo with `git revert e6a371c14`.

### 34. Auto Ban button only as wide as needed, version v1.6.1 — commit `c7d689a4e` (2026-10-19)

Fix for change 33: the Auto Ban button was stretched very wide, because the layout holding
the mute checkbox had a stretchable gap, which the toolbar treats as "may grow". The
checkbox is now aligned right without a stretch, the button's size policy is fixed, and its
right padding is the checkbox's measured width plus small margins. The button is as wide as
"Auto Ban" plus the checkbox and speaker.

- Changed: `src/gui/mainwindow.cpp`, `src/base/version.h.in` (v1.6.1),
  `docs/mankkas-edition/USER_GUIDE.md` (version only)
- Undo with `git revert c7d689a4e`.

### 33. Mute checkbox in the Auto Ban button, version v1.6.0 — commit `f0debdcb0` (2026-10-19)

A checkbox with a speaker icon sits inside the Auto Ban button, to the right of its text.
Unchecked (speaker with sound waves): Auto Ban shows the ban box. Checked (speaker with a
cross): Auto Ban shows no ban box, both for real-time bans and for the check when it is
turned on. Banning, logging and counting are unchanged, and the Ban button always shows
its box. Clicking the checkbox only changes the mute; the rest of the button still toggles
Auto Ban. The setting is remembered.

- New files: `src/icons/speaker.svg`, `src/icons/speaker-muted.svg` (white, for the red
  and green button)
- Changed: `src/gui/mainwindow.cpp/.h` (`banPeersFromCheckedCountries(bool byAutoBan)`,
  `isAutoBanMuted()`), `src/icons/icons.qrc`, `src/gui/uithemecommon.h`,
  `src/base/version.h.in` (v1.6.0), `docs/mankkas-edition/USER_GUIDE.md`
- Setting in `qBittorrent.ini` under `[GUI]`: `BanCountries\AutoBanMuted`.
- Undo with `git revert f0debdcb0`.

### 32. "From Me" column in the Peers tab, version v1.5.0 — commit `154e8415e` (2026-10-19)

A new Peers tab column "From Me": what the peer has downloaded from us (the same counter
as "Uploaded", for the current connection) as a percentage of the torrent's total size.
It sorts, hides, moves and resizes like the other columns and follows "hide zero values".

The column (`PeerListColumns::FROM_ME`) is added **after** the internal `IP_HIDDEN` column,
so every existing column keeps its number and saved Peers layouts restore unchanged; the
new column appears at the far right. The loops that ended at `IP_HIDDEN` (at least one
visible column, sizing columns with width 0, the column menu) now cover all columns
except `IP_HIDDEN`.

- Changed: `src/gui/properties/peerlistwidget.cpp/.h`, `src/base/version.h.in` (v1.5.0),
  `docs/mankkas-edition/USER_GUIDE.md`
- Undo with `git revert 154e8415e`. Saved layouts then have one column too many, which Qt
  may handle by resetting the Peers layout once.

### 31. "Blocked today by IP filter" in the Total bans window, version v1.4.0 — commit `f476e07b6` (2026-10-19)

Below "Total:" the Total bans window shows "Blocked today by IP filter: nnnn", the number of
connections the IP filter (the banned IP list) refused today. The main window counts the
peer log entries that were blocked with the reason "IP filter" (`Logger::newLogPeer`, plus
the ones logged before the window was created). The count is stored like the "Today:" ban
counter, so it survives restarts and starts from 0 on a new date. The window refreshes it
every 2 seconds.

- Changed: `src/gui/bancounter.cpp/.h`, `src/gui/mainwindow.cpp`,
  `src/gui/totalbansdialog.cpp/.h`, `src/base/version.h.in` (v1.4.0),
  `docs/mankkas-edition/USER_GUIDE.md`
- Settings in `qBittorrent.ini` under `[GUI]`: `BanCountries\BlockedTodayDate`,
  `BanCountries\BlockedTodayCount`. Safe to delete after undoing.
- Undo with `git revert f476e07b6`.

### 30. Active upload peers in the status bar, version v1.3.0 — commit `8485c17b7` (2026-10-19)

The upload speed field at the bottom of the window ends with "(nn Peers)": the number of
peers being uploaded to right now, over all torrents. On each status bar refresh,
`StatusBar::updateActiveUploadPeers()` fetches the peer lists of the torrents that upload
at that moment and counts the peers with an upload speed above 0. The field shows the
latest finished count.

- Changed: `src/gui/statusbar.cpp/.h`, `src/base/version.h.in` (v1.3.0),
  `docs/mankkas-edition/USER_GUIDE.md`
- Undo with `git revert 8485c17b7`.

### 29. Ban box higher above the taskbar, version v1.2.2 — commit `ee5880569` (2026-10-19)

The box shown after Ban / Auto Ban sits 64 pixels above the bottom of the usable screen area
(was 16), so it stays clear of the taskbar. It is still 16 pixels from the right edge.

- Changed: `src/gui/mainwindow.cpp`, `src/base/version.h.in` (v1.2.2),
  `docs/mankkas-edition/USER_GUIDE.md`
- Undo with `git revert ee5880569`.

### 28. Ban box never takes the focus, lower right corner, version v1.2.1 — commit `40691e0fb` (2026-10-19)

The box shown after Ban / Auto Ban no longer affects the window you're working in. It
used to belong to qBittorrent's main window, so showing or closing it could bring
qBittorrent to the front. Now it has no owner window, can't be activated
(`Qt::WindowDoesNotAcceptFocus`, shown without activating) and lets mouse clicks through
(`Qt::WindowTransparentForInput`). It appears in the **lower right** corner above the
taskbar (was lower left), on the screen of qBittorrent's window, or on the main screen
while qBittorrent is hidden in the tray.

- Changed: `src/gui/mainwindow.cpp`, `src/base/version.h.in` (v1.2.1),
  `docs/mankkas-edition/USER_GUIDE.md`
- Undo with `git revert 40691e0fb`.

### 27. English is the default language, version v1.2.0 — commit `e9b89f6d9` (2026-10-18)

Until the user chooses a language (Tools → Options → Behavior → Language), the program and
the Web UI are in English instead of following the language of Windows. A language that was
already chosen (the `[Preferences] General\Locale` setting) is kept.

- Changed: `src/base/preferences.cpp` (`Preferences::getLocale()`), `src/base/version.h.in`
- Undo with `git revert e9b89f6d9` (the default then follows Windows again).

### 26. Crash reports go to the edition, version v1.1.2 — commit `7ea484fd9` (2026-10-18)

If the program crashes, the crash window now says "qBittorrent Mankka's Edition has crashed",
asks for a bug report at the edition's GitHub Issues (`QBT_MOD_GITHUB_URL`/issues) instead of
qBittorrent's bug tracker, asks not to report it to the official project, and shows the
edition's name and version above the technical details.

- Changed: `src/gui/stacktracedialog.cpp`, `src/base/version.h.in`
- Undo with `git revert 7ea484fd9`.

### 25. Qt's translations in the build and the release zip (packaging only) — commit `601b26487` (2026-10-18)

Qt's own buttons and standard dialogs now follow the program's language (e.g. Finnish):
`build-windows.ps1` and `package-release.ps1` copy Qt's `translations\qtbase_*.qm` next to
the program (the `translations` folder named in `qt.conf`). Qt's `qttranslations` archive must
be installed (BUILDING.md shows the command). The program itself is unchanged, so the version
stays v1.1.1.

- Changed: `dist/mankkas-edition/build-windows.ps1`, `dist/mankkas-edition/package-release.ps1`,
  `docs/mankkas-edition/BUILDING.md`
- Undo with `git revert 601b26487`.

### 24. Update check and About links lead to the edition, version v1.1.1 — commit `00a5136f4` (2026-10-18)

- **Update check** (Help → Check for Updates and the automatic check) asks GitHub for the
  latest release of the edition (`QBT_MOD_GITHUB_REPO` = `mankka-beep/qBittorrent-Mankkas-Edition`
  in `src/base/version.h.in`) and compares its tag (e.g. `v1.1.2`) with `QBT_MOD_VERSION`.
  It never looks at or offers the official qBittorrent any more. "Yes" and "Open
  changelog..." open the edition's release page. Without releases (or without internet) it
  reports no update and writes a warning to the log.
- **About** (program and Web UI): the Forum and Bug Tracker links now lead to the edition's
  GitHub page and its Issues; the qBittorrent home page link stays. The Web UI About also
  says the edition is unofficial. Help → Donate still goes to qBittorrent.
- If the repository gets a different name, change `QBT_MOD_GITHUB_REPO` **and** the two links
  in `src/webui/www/private/views/about.html`, then rebuild.
- Changed: `src/base/version.h.in`, `src/gui/programupdater.cpp/.h`, `src/gui/mainwindow.cpp`,
  `src/gui/aboutdialog.cpp`, `src/webui/www/private/views/about.html`
- Undo with `git revert 00a5136f4` (the program would offer official qBittorrent updates again).

### 23. Repository and release preparation (no change to the program) — commits `9ea8abfc2`, `c83fe8b19`, `f0be73ed3`, `16781ac78`, `f4272c860` (2026-10-18)

- `README.md` is now the edition's front page (with download, install, feedback and build
  sections); qBittorrent's own README is kept as `README.qBittorrent.md`.
- `.github/SUPPORT.md`, `SECURITY.md`, `CONTRIBUTING.md`, the pull request template and the
  issue forms ("Bug report", "Idea / feature request") describe the edition and ask people
  not to report it to the official project. qBittorrent's funding link is removed.
- qBittorrent's GitHub Actions workflows and Dependabot are removed; only the pre-commit
  helper scripts in `.github/workflows/helper/pre-commit` are kept.
- `dist/mankkas-edition/build-windows.ps1` builds the edition (folders as parameters,
  Boost/libtorrent downloaded when missing); `docs/mankkas-edition/BUILDING.md` explains it.
- `dist/mankkas-edition/package-release.ps1` makes
  `release\qBittorrent-Mankkas-Edition-vX.Y.Z-win64.zip` with the program, Qt, the Visual
  C++ runtime, the licences (`dist/mankkas-edition/licenses`), README, user guide and this
  log. `release/` is ignored by git.
- Undo with `git revert` of the commits above (newest first).

### 22. Named "Mankka's Edition", version v1.1.0 — commit `a5f6a3b9d` (2026-10-18)

The window title and the About dialog show "qBittorrent v1.1.0 Mankka's Edition"
(previously "qBittorrent v1.0.0 Mankka © 2026"). The About dialog adds "Modifications
© 2026 Mankka" and "Unofficial edition, not affiliated with the qBittorrent project."
The version numbering rules above are written down in `src/base/version.h.in`. Trackers
and peers still see qBittorrent 5.4.0.

- Changed: `src/base/version.h.in`, `src/gui/aboutdialog.cpp`
- Undo with `git revert a5f6a3b9d`.

### 21. Sort the Progress column by the progress shown — commit `42acafeaf` (2026-10-18)

Sorting by Progress now uses the value the bar shows: for a seed that is uploading, the
highest progress of the peers it uploads to; for all other torrents, their own progress.
Uploading seeds therefore move in the sorted list as their peers' progress changes.

- Changed: `src/gui/transferlistsortmodel.cpp`
- Undo with `git revert 42acafeaf` (sorting then uses the torrents' own progress again).

### 20. Own version name, lighter green, peer progress on uploading seeds — commit `ea3706895` (2026-10-18)

- **Name:** the window title and the About dialog show "qBittorrent v1.0.0 Mankka © 2026"
  (`QBT_DISPLAY_NAME` / `QBT_MOD_VERSION` in `src/base/version.h.in`). The About dialog
  adds "Based on qBittorrent v5.4.0alpha1". The program still reports itself as
  qBittorrent 5.4.0 to trackers and peers, so trackers that only allow known clients keep
  working.
- **Arrows:** the uploading arrows are the "Running" green `#32cd32` (was `#008000`).
- **Progress bar:** while a seeding torrent is uploading (green arrows), the Progress bar
  shows the highest progress of the peers it is uploading to, drawn in `#32cd32`. The
  peers are read from the torrent's peer list on each list refresh, only for uploading
  torrents. Downloading, idle or stopped torrents show their own progress as before.
  Sorting by Progress still uses the torrent's own progress. No columns were added,
  removed or moved.
- Changed: `src/base/version.h.in`, `src/gui/aboutdialog.cpp`, `src/gui/mainwindow.cpp`,
  `src/gui/transferlistmodel.cpp/.h`, `src/gui/transferlistdelegate.cpp`,
  `src/icons/uploading.svg`
- Undo with `git revert ea3706895`.

### 19. "Unban" button in the "Total bans" window — commit `5541d10b7` (2026-10-18)

An "Unban" button in the lower left corner of the "Total bans" window (Close stays on the
right). It's greyed out until a country is selected; several can be selected with Ctrl or
Shift. Pressing it removes every banned IP address of the selected countries from the
banned IP list right away (no confirmation), writes "Unbanned N IP address(es) from …"
to the execution log, and recounts the list. Countries still checked in the dropdown
can be banned again by Ban or Auto Ban.

- Changed: `src/gui/totalbansdialog.cpp/.h`
- Undo with `git revert 5541d10b7`. IPs already unbanned stay unbanned.

### 18. Bans only apply to completed (seeding) torrents — commit `8b600332c` (2026-10-18)

Banned IPs are only blocked for torrents that are complete (seeding). A torrent that is
still downloading ignores the ban list, so banned peers can connect to it. This uses
libtorrent's per-torrent `apply_ip_filter` flag, which `TorrentImpl::updateIPFilterUsage()`
keeps in step with the torrent's state on every status update. When a torrent finishes, the
bans start applying to it and banned peers are disconnected. If it downloads again (e.g.
more files selected), the bans stop applying again. Seeding torrents still refuse banned
IPs and log them in Blocked IPs. Ban, Auto Ban and the Peers tab still add IPs to the ban
list as before; it just only takes effect for seeding torrents.

The session's IP filter holds only the banned IPs here (no IP filter file is in use). If
an IP filter file is enabled later, downloading torrents would ignore it too.

- Changed: `src/base/bittorrent/torrentimpl.cpp/.h`
- **How to undo safely:** libtorrent saves the flag with each torrent, so a plain revert would
  leave the torrents that were downloading at that moment ignoring bans. Instead:
  1. In `TorrentImpl::updateIPFilterUsage()` change `const bool shouldApplyIPFilter = isFinished();`
     to `const bool shouldApplyIPFilter = true;`, rebuild and run qBittorrent for a minute with
     all torrents started.
  2. Then `git checkout -- src/base/bittorrent/torrentimpl.cpp` and `git revert 8b600332c`,
     and rebuild.

### 17. Real-time Auto Ban, 3-second box, country tooltips on flags — commit `513857986` (2026-10-18)

- **Auto Ban is real-time.** While it's green, qBittorrent reports every new peer
  connection (incoming and outgoing) and a peer from a checked country is banned right
  away. That's the same ban as before (ban list, log entry, "Today:" count), now
  without waiting a minute. When turned on, the peers that are already connected are
  checked once. The once-a-minute timer is gone. Real-time Auto Ban uses the loaded GeoIP
  database, so "Resolve peer countries" must stay on.
- Engine part: `Session::setPeerConnectedNotificationEnabled()` turns libtorrent's
  connection alerts on only while Auto Ban is on, and `Session::peerConnected(ip)` reports
  each connection.
- **The ban box** shows for 3 seconds (was 5), for both Ban and Auto Ban. For Auto Ban it
  says "<IP> from <country> banned". A new box replaces the previous one instead of
  piling up.
- **Tooltips:** hovering over a flag in Execution Log or Blocked IPs shows the country name.
- Changed: `src/base/bittorrent/session.h`, `sessionimpl.cpp/.h`,
  `src/gui/mainwindow.cpp/.h`, `src/gui/log/loglistview.cpp`
- Undo with `git revert 513857986` (Auto Ban then runs once a minute again).

### 16. Flags after banned IPs in the Execution Log tab — commit `7d3d19e62` (2026-10-18)

In Execution Log → Execution Log, messages about banned peers show the IP's country
flag right after the IP address, drawn the same way as in the Blocked IPs tab. The
messages recognized (also when translated) are `Peer "<ip>" is manually banned`
(Peers tab, Ban, Auto Ban; the flag goes after the closing quote) and
`IP banned by libtorrent. IP: <ip>.`. Other messages are unchanged.

- Changed: `src/gui/log/logmodel.cpp`, `src/gui/log/loglistview.cpp`
- Undo with `git revert 7d3d19e62`.

### 15. Ban box in the lower left corner, flags in the Blocked IPs tab — commit `c2bb22aa7` (2026-10-18)

- The 5-second result box after "Ban" / "Auto Ban" now appears in the lower left
  corner of the screen (above the taskbar) instead of the middle. The "Total bans"
  window is unchanged.
- In Execution Log → Blocked IPs, each row shows the IP's country flag right after
  the IP address, sized to the row height. The country comes from the same GeoIP
  database as the Peers tab, so flags only appear while "Resolve peer countries" is on.
  Copying rows (Ctrl+C) still copies plain text.
- Changed: `src/gui/mainwindow.cpp/.h`, `src/gui/log/logmodel.cpp/.h`,
  `src/gui/log/loglistview.cpp`
- Undo with `git revert c2bb22aa7`.

### 14. "Select All" toolbar button — commit `41691a5b1` (2026-10-17)

A toolbar button left of the country dropdown, in the same style and size as the other
toolbar buttons. It selects all torrents in the list, the same as Ctrl+A, switching to
the Transfers tab first if another tab is open. Its icon is new: three highlighted list
rows with ticked checkboxes, in qBittorrent's usual blue.

- New file: `src/icons/select-all.svg`
- Changed: `src/icons/icons.qrc`, `src/gui/uithemecommon.h`, `src/gui/mainwindow.cpp`
- Undo with `git revert 41691a5b1`.

### 13. No confirmation when pressing "Ban" — commit `4c93e3433` (2026-10-17)

Pressing "Ban" bans the peers from the checked countries right away, without the
"Are you sure…?" question, the same as Auto Ban. The log entries and the result box
with "Today:" are unchanged. No ban anywhere in the program asks for confirmation now.

- Changed: `src/gui/mainwindow.cpp/.h`
- Undo with `git revert 4c93e3433`.

### 12. "Auto Ban" button — commit `9c6350d46` (2026-10-17)

An "Auto Ban" toggle button right of "Ban". Red = off, green = on; each press switches.
It always starts red (off) when the program starts. While it's on, the ban routine
runs right away and then once a minute, exactly like pressing "Ban" (same log entries,
same result box with "Today:"), except that it doesn't ask "Are you sure…?". Turning
it on is the confirmation. A run is skipped if no country is checked or the previous
ban is still collecting peers. The "already banned?" check now uses a set, so it stays
fast with 10,000+ bans.

- Changed: `src/gui/mainwindow.cpp/.h`
- No settings stored.
- Undo with `git revert 9c6350d46`.

### 11. "Total bans" button and window — commit `886dca9f0` (2026-10-17)

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
- Undo with `git revert 886dca9f0`.

### 10. "Today:" ban counter in the ban notification — commit `a696286f3` (2026-10-17)

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
- Undo with `git revert a696286f3`.

### 9. Checked countries are remembered — commit `9aea69727` (2026-10-16)

The countries checked in the country dropdown stay checked after closing and
restarting qBittorrent, and after rebuilding it. They are saved every time a
country is checked or unchecked. Ban is enabled at startup if any are checked.

- Changed: `src/gui/countrycombobox.cpp/.h`, `src/gui/mainwindow.cpp/.h`
- Settings key: `GUI\BanCountries\Checked` (list of country codes)
- Undo with `git revert 9aea69727`; the key can then be deleted.

### 8. Idle force-started seeds keep blue arrows — commit `e1561e8c9` (2026-10-16)

Fix for change 7: a force-started seeding torrent counts as "uploading" all the
time, so it showed green arrows even when idle. Now it shows green only while
something is actually being uploaded, otherwise the light blue idle arrows.

- Changed: `src/gui/transferlistmodel.cpp`
- Undo with `git revert e1561e8c9` (revert before change 7).

### 7. Green arrows for actively uploading torrents — commit `f76071506` (2026-10-16)

In the torrent list, torrents that are uploading right now show green up-arrows
instead of blue, the same green as downloading torrents. Idle seeding torrents
(nobody downloading from you) keep their light blue arrows. The upload speed in
the status bar and the "Seeding" filter keep the original blue icon.

- New file: `src/icons/uploading.svg` (copy of `upload.svg`, colour `#008000`)
- Changed: `src/icons/icons.qrc`, `src/gui/uithemecommon.h`, `src/gui/transferlistmodel.cpp`
- Undo with `git revert f76071506`.

### 6. Check several countries, ban them all at once — commit `809bb99f9` (2026-10-16)

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
- Undo with `git revert 809bb99f9`: the dropdown goes back to choosing one country.

### 5. Bandwidth priority column removed — commit `63d13e6e3` (2026-10-16)

Reverts change 3 completely on request: the "#" checkbox column and
`bandwidthprioritizer.h/.cpp` are gone, and the other files are back as before.
No torrents were checked at the time, so every torrent already had its own speed
limits. The feature's keys were removed from `qBittorrent.ini` (backup:
`qBittorrent.ini.before-priority-removal.bak`). Change 4 is unaffected.

To bring the feature back: `git revert 63d13e6e3`, then rebuild.

### 4. No confirmation when banning peers from the Peers tab — commit `0362954d8` (2026-10-15)

"Ban peer permanently" in the Peers tab (right-click menu or Delete key) bans the
selected peers right away, without the "Are you sure…?" question. The execution log
entry is unchanged. The country "Ban" button still asks first.

- Changed: `src/gui/properties/peerlistwidget.cpp`

### 3. Bandwidth priority column ("#") — commit `525552869` (2026-10-15) — REMOVED in change 5

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

### 2. Ban button, list scroll bar — commit `0ac601390` (2026-10-15)

"Ban" button right of the country dropdown: bans all connected peers from the
selected country, with the same confirmation and execution log entry as manual
banning, plus a 5-second notification in the middle of the screen. The country
list opens with a scroll bar.

- Changed: `src/base/net/geoipmanager.cpp/.h`, `src/gui/mainwindow.cpp/.h`
- Bans are stored in qBittorrent's normal ban list. Reverting the code does not
  remove them; use Options → Connection → IP Filtering → Manually banned IP
  addresses.

### 1. Country dropdown — commit `e5c29548f` (2026-10-14)

Dropdown right of the Options button listing every country in
`dbip-country-lite.mmdb`, with flags, sorted alphabetically.

- Changed: `src/base/net/geoipdatabase.cpp/.h`, `src/base/net/geoipmanager.cpp/.h`,
  `src/gui/mainwindow.cpp/.h`
