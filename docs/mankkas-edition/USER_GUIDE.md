# qBittorrent Mankka's Edition — User Guide

Version **v1.5.0**, based on qBittorrent v5.4.0alpha1.

Mankka's Edition is an unofficial, modified version of qBittorrent for Windows. It adds
tools for blocking peers by country, quicker banning, ban statistics, and a few visual
changes to the torrent list. Everything else works exactly like normal qBittorrent, so
the official qBittorrent documentation still applies to all other features.

---

## Contents

1. [The toolbar at a glance](#1-the-toolbar-at-a-glance)
2. [Select All](#2-select-all)
3. [Country list](#3-country-list)
4. [Ban](#4-ban)
5. [Auto Ban](#5-auto-ban)
6. [The ban result box and the "Today" counter](#6-the-ban-result-box-and-the-today-counter)
7. [Banning peers in the Peers tab](#7-banning-peers-in-the-peers-tab)
8. [Total bans and Unban](#8-total-bans-and-unban)
9. [Bans only apply to seeding torrents](#9-bans-only-apply-to-seeding-torrents)
10. [The torrent list: green arrows and peer progress](#10-the-torrent-list-green-arrows-and-peer-progress)
11. [Flags in the Execution Log](#11-flags-in-the-execution-log)
12. [Name, version and About](#12-name-version-and-about)
13. [Tips and troubleshooting](#13-tips-and-troubleshooting)
14. [Settings stored by the edition](#14-settings-stored-by-the-edition)

---

## 1. The toolbar at a glance

Next to the usual toolbar buttons you'll find, from left to right:

| Item | What it does |
|---|---|
| **Select All** (blue list icon) | Selects all torrents, like Ctrl+A |
| **Country list** | Choose the countries whose peers you want to ban |
| **Ban** | Bans the connected peers from the checked countries, once |
| **Auto Ban** (red/green) | Keeps banning peers from the checked countries as soon as they connect |
| **Total bans** (next to "Filter torrents...") | Shows your banned IPs per country, with an Unban button |

Countries are recognised with the free *IP to Country Lite* database by DB-IP, which
qBittorrent downloads and updates by itself.

> **Important:** the country features need **Options → Connection → "Resolve peer
> countries"** to be turned on (it is on by default).

---

## 2. Select All

The blue button with three ticked rows, just left of the country list, selects every
torrent in the list — the same as pressing **Ctrl+A** in the torrent list. If you're on
another tab (Search, RSS, …) it switches to the Transfers tab first. Hover over it to see
"Select all torrents (Ctrl+A)".

---

## 3. Country list

The drop-down list right of Select All contains every country found in the GeoIP
database, sorted alphabetically, each with its flag.

- **Check a country** by clicking it, or by pressing **Space** on it. The list stays open,
  so you can check as many countries as you like.
- Close the list with **Enter** or by clicking outside it.
- The closed list shows **"Select countries"**, the one checked country with its flag, or
  **"N countries"**. Hover over it to see which ones are checked.
- Your checked countries are **remembered** when you close qBittorrent and when you update
  to a newer build.

---

## 4. Ban

**Ban** goes through the peers that are connected right now, in all torrents, and
permanently bans every one whose IP address belongs to a checked country.

- It's greyed out until at least one country is checked.
- There is **no confirmation question** — it bans right away.
- Peers that are already banned and I2P peers (which have no IP address) are skipped.
- Each banned IP is written to the Execution Log as `Peer "1.2.3.4" is manually banned`,
  the same entry as a ban from the Peers tab.
- Afterwards the [result box](#6-the-ban-result-box-and-the-today-counter) shows how many
  peers were banned.

Bans go into qBittorrent's normal banned IP list (**Options → Connection → IP Filtering →
Manually banned IP addresses**), so they stay after a restart.

---

## 5. Auto Ban

**Auto Ban** is an on/off button:

- **Red** = off. It is always red when qBittorrent starts.
- **Green** = on. Click it again to turn it back to red.

While it's green:

1. The peers already connected are checked once right away (like pressing Ban).
2. From then on, **every new peer connection is checked the moment it's made** — both
   peers connecting to you and peers you connect to. A peer from a checked country is
   banned immediately.

Each Auto Ban shows a result box such as "1.2.3.4 from Sweden banned", and is logged and
counted like any other ban.

Good to know:

- Your existing banned list is always checked first. A peer that is already banned never
  gets far enough to reach Auto Ban; Auto Ban only deals with new IPs.
- A peer from a checked country may be connected for a split second before Auto Ban cuts
  it off; after that it's on the banned list like any other ban.
- If you uncheck a country, Auto Ban stops banning it straight away.

---

## 6. The ban result box and the "Today" counter

After Ban, Auto Ban or a ban from the Peers tab the edition keeps count, and after **Ban**
and **Auto Ban** a box appears in the **lower right corner** of the screen, a little above the
taskbar, for **3 seconds**:

```
 12 peer(s) banned
 Sweden: 5
 Germany: 7
 ──────────────
 Today: 148
```

- With one checked country it reads e.g. "5 peer(s) from Sweden banned"; with several, it
  shows the total plus a line per country that had peers banned.
- **Today:** below the line is the number of peers banned **today** — with Ban, Auto Ban
  and the Peers tab together. Each IP counts once; IPs that were already banned don't
  count. The counter is kept when you restart and starts again from 0 when the date
  changes.
- When several bans happen quickly, each new box replaces the previous one instead of
  piling up.
- The box never takes the focus and never brings qBittorrent (or any other window) to the
  front, so you can keep working in another program while Auto Ban runs. Mouse clicks go
  through it. It appears on the screen qBittorrent's window is on, or on the main screen
  while qBittorrent is hidden in the tray.

---

## 7. Banning peers in the Peers tab

In a torrent's **Peers** tab, right-click → **Ban peer permanently** (or press
**Delete**) bans the selected peers **right away, without a confirmation question**.
They are logged as before and added to the "Today" counter.

> Because there's no question any more, pressing Delete in the Peers tab bans
> immediately. Bans can be undone in Options → Connection → IP Filtering, or with
> [Unban](#8-total-bans-and-unban).

**"From Me" column:** the Peers tab has an extra column, **From Me**, showing how much
of the torrent the peer has downloaded **from you**, as a percentage of the torrent's
total size. Example: a peer that has downloaded 450 MB from you of a 900 MB torrent shows
**50.0%**. It works like the other columns: sort by it, hide or show it (right-click the
column headers), move and resize it. It was added at the far right, so your existing
column layout stays as it was.

The value counts what you have sent to that peer during its current connection (the same
data as the "Uploaded" column); if the peer reconnects, it starts again from 0.

---

## 8. Total bans and Unban

The **Total bans** button, just left of the "Filter torrents..." box (shown on the
Transfers tab), opens a window listing all your banned IP addresses **per country**:

- Each row shows the flag, country and number of bans, **most bans first** (countries with
  the same number in alphabetical order).
- IPs the database can't place are counted under **N/A**.
- The bold line at the bottom shows the total, e.g. "Total: 11 329 bans in 87 countries".
- Below it, **"Blocked today by IP filter: nnnn"** shows how many connections the IP filter
  (your banned IP list) has refused today — the "IP filter" entries in the Blocked IPs log.
  It's updated every 2 seconds while the window is open, is kept when you restart, and starts
  again from 0 when the date changes.
- **Unban** (lower left corner): select one or more countries (Ctrl/Shift for several) and
  press Unban to remove **all** banned IPs of those countries from the banned list at
  once. The list recounts immediately, and the Execution Log gets a line like "Unbanned 71
  IP address(es) from Sweden".
- **Close** (lower right) or Esc closes the window. It remembers its size.

> If you unban a country that is still checked in the country list, Ban and Auto Ban
> will ban its peers again. Uncheck it there too if you want it to stay unbanned.

---

## 9. Bans only apply to seeding torrents

In this edition the banned IP list only applies to **completed (seeding) torrents**:

- **Downloading torrents** ignore the banned list. Banned peers can connect to them, so
  your downloads keep every possible source.
- **Seeding torrents** refuse banned IPs as usual; refused attempts appear in the Blocked
  IPs tab.
- This switches automatically: when a download finishes, the bans start applying to it
  (and banned peers on it are disconnected). If it starts downloading again — for example
  because you selected more files — the bans stop applying to it again.

Ban, Auto Ban and the Peers tab still add IPs to the banned list as normal; a ban simply
only takes effect on your seeding torrents.

> qBittorrent's own protection against peers that send **corrupt data** ("IP banned by
> libtorrent" in the log) is unchanged and applies to all torrents.

---

## 10. The torrent list: green arrows and peer progress

**Arrows in front of the torrent name:**

| Arrows | Meaning |
|---|---|
| **Lime green** up-arrows (the same green as "Running" in the Status list) | Seeding and uploading right now |
| Light blue up-arrows | Seeding, but nobody is downloading from you at the moment |

A force-started seed ("[F] Seeding") also shows blue while idle and green only while it
actually uploads.

**Progress column:**

- While a seeding torrent is uploading (green arrows), its Progress bar shows the
  **highest progress of the peers you're uploading to**, drawn in the same lime green.
  For example, "37.4%" means the most complete peer you're currently sending to has 37.4%.
- Downloading, idle and stopped torrents show their own progress as normal.
- **Sorting** by the Progress column uses the value the bar shows, so uploading seeds move
  in the list as their peers' progress changes.

**Status bar (bottom of the window):** the upload speed field ends with the number of peers
you are uploading to right now, over all torrents (seeding and downloading):

```
U: 512 KiB/s (3,1 GiB) (12 Peers)
```

It is updated each time the status bar refreshes.

---

## 11. Flags in the Execution Log

Open **View → Log** (the Execution Log at the bottom of the window):

- **Blocked IPs tab:** every row shows the country flag right after the IP address, e.g.
  `1.2.3.4 🇸🇪 was blocked. Reason: IP filter.`
- **Execution Log tab:** messages about banned peers show the flag after the IP:
  `Peer "1.2.3.4" 🇸🇪 is manually banned` and `IP banned by libtorrent. IP: 1.2.3.4 🇸🇪.`
- **Hover over a flag** to see the country name.
- Copying rows (Ctrl+C) still copies plain text.

---

## 12. Name, version and About

- The window title shows **"qBittorrent v1.5.0 Mankka's Edition"**.
- **Help → About** shows the edition name, the qBittorrent version it's based on, and
  "Modifications © 2026 Mankka". Its links lead to the qBittorrent home page, the
  edition's own GitHub page, and the edition's bug tracker (Issues).
- To trackers and other peers the program still identifies itself as qBittorrent 5.4.0,
  because many trackers only accept known qBittorrent versions.
- **Version numbers:** the second number goes up with each new feature or visible change,
  the third with each fix, and the first when the edition moves to a newer official
  qBittorrent release.

**Updates.** **Help → Check for Updates** — and the automatic check, if "Check for program
updates" is on in Options → Behavior — looks for new releases of **Mankka's Edition** on
its GitHub page, never for the official qBittorrent. When a newer edition is available,
it asks whether you want to download it and opens the release page. Download the zip and
unzip it over your old folder (with qBittorrent closed). **Help → Donate** still goes to
the official qBittorrent's donation page, to support the original project.

---

## 13. Tips and troubleshooting

**Language.** The edition starts in English, whatever language Windows uses. Choose your
language in **Tools → Options → Behavior → Language** and restart the program; the Web
UI follows the same setting. A language chosen earlier in qBittorrent is kept.

**"All my changes are gone" / the toolbar buttons are missing.** Only one qBittorrent can
run at a time. If the official qBittorrent is installed and was started first (for
example by opening a .torrent file, which Windows may still send to the official
version), starting Mankka's Edition just brings the official window forward. Exit the
other qBittorrent completely (tray icon → Exit) and start Mankka's Edition again. The
window title tells you which one is running.

**No flags, and Auto Ban doesn't ban.** Turn on Options → Connection → "Resolve peer
countries". On the very first start the GeoIP database may still be downloading.

**A country I unbanned gets banned again.** It's still checked in the country list.

**Undoing bans.** Use Unban in the Total bans window (whole countries), or Options →
Connection → IP Filtering → Manually banned IP addresses (single IPs).

---

## 14. Settings stored by the edition

Besides qBittorrent's normal settings, the edition stores these in
`%APPDATA%\qBittorrent\qBittorrent.ini`:

| Setting | Purpose |
|---|---|
| `[GUI] BanCountries\Checked` | Your checked countries |
| `[GUI] BanCountries\TodayDate`, `BanCountries\TodayCount` | The "Today" ban counter |
| `[GUI] BanCountries\BlockedTodayDate`, `BanCountries\BlockedTodayCount` | "Blocked today by IP filter" |
| `[TotalBansDialog] Size` | Size of the Total bans window |

The banned IPs themselves are stored in qBittorrent's normal setting (`[State] BannedIPs`).
