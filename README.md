# qBittorrent Mankka's Edition

An **unofficial**, modified version of [qBittorrent](https://www.qbittorrent.org/) for
Windows with extra tools for blocking peers by country, faster banning and ban
statistics.

> **This is not the official qBittorrent.** It is not affiliated with or endorsed by the
> qBittorrent project. Please don't report problems with this edition to them.

Current version: **v1.1.1**, based on qBittorrent v5.4.0alpha1.

## Why I made this version?

I made this to develop qBittorrent for my own needs, as I share a lot, but then I realized that maybe someone else has been thinking about the same issues.

This first version is heavily focusing on banning uploading peers & based on my own needs, why?

For example: I got tired of Chinese bots taking up all my upload slots just sitting there while using my bandwidth in the background.

But you are of course free to use it in your own way for your own purposes.

It also has some neat improvements to the application to make your life easier and more in control of what is happening, especially when seeding.

I'm totally open to all improvements, criticism, new ideas and so on, I'm all ears!

If you have good & creative ideas how to improve the application they are very much appreciated & I'll see if I'm able to implement them.

Keep on Torrenting!

Mankka

## What's different from qBittorrent

- **Country list** with checkboxes and flags: choose the countries whose peers you want to ban.
- **Ban** button: bans all connected peers from the checked countries at once.
- **Auto Ban** (red/green): bans peers from the checked countries the moment they connect.
- **Ban result box** in the lower left corner with a **"Today"** counter of bans.
- **Total bans** window: banned IPs per country, with **Unban** for whole countries.
- **Bans only apply to seeding torrents**; downloads can use every available peer.
- **No confirmation** when banning peers from the Peers tab.
- **Select All** toolbar button.
- **Lime green arrows** for torrents that are uploading right now, and a **Progress bar
  showing the peers' progress** while seeding (sortable).
- **Country flags** (with country names on hover) after IP addresses in the Execution Log
  and Blocked IPs tabs.

See the [user guide](docs/mankkas-edition/USER_GUIDE.md) for how everything works, and
[LOCAL_CHANGES.md](LOCAL_CHANGES.md) for every change in detail.

## Download and install (Windows 10/11, 64-bit)

1. Download the newest `qBittorrent-Mankkas-Edition-vX.Y.Z-win64.zip` from the
   **Releases** page of this repository.
2. Unzip it to a folder of your choice, e.g. `C:\Programs\qBittorrent Mankka's Edition`.
3. Start `qbittorrent.exe` from that folder. No installation is needed.

To update: **Help → Check for Updates** tells you when a new release of this edition is
out (it never offers the official qBittorrent). Download the new zip and unzip it over the
old folder while qBittorrent is closed.

Good to know:

- The program isn't digitally signed, so **Windows SmartScreen may warn you** the first
  time ("Windows protected your PC"). Click **More info → Run anyway** if you trust it.
- It uses the **same settings and torrent list as the official qBittorrent**
  (`%APPDATA%\qBittorrent` and `%LOCALAPPDATA%\qBittorrent`). Back them up before trying
  it. It is based on a newer development version of qBittorrent, so an older official
  version may not read everything back correctly afterwards.
- Only one qBittorrent can run at a time. If the official qBittorrent is already
  running, close it completely (tray icon → Exit) before starting this edition — the
  window title shows which one is running.

## Things to know

- To trackers and peers the program identifies itself as qBittorrent 5.4.0. **Some
  private trackers don't allow modified clients** — check your trackers' rules first.
- It is based on qBittorrent **v5.4.0alpha1**, a development version of qBittorrent, not
  a finished release.
- Country detection uses the free IP to Country Lite database by
  [DB-IP](https://db-ip.com/) (CC BY 4.0).
- Much of this edition was written with the help of Claude, an AI assistant, and
  reviewed and tested by me.
- Provided as is, without warranty.

## Feedback, ideas and problems

Use the **Issues** tab of this repository — ideas are very welcome! See
[SUPPORT.md](.github/SUPPORT.md) and [CONTRIBUTING.md](CONTRIBUTING.md).

**Please don't report anything about this edition to the official qBittorrent project.**
They don't support modified versions.

## Building from source

See [BUILDING.md](docs/mankkas-edition/BUILDING.md).

## Licence

Like qBittorrent, this edition is free software under the GNU General Public License:
the source code is GPLv2 or later, the compiled program GPLv3 or later, with an
exception for linking OpenSSL. See [COPYING](COPYING). qBittorrent is © its contributors
(see [AUTHORS](AUTHORS)); the modifications are © 2026 Mankka. The licences of the
libraries in the Windows download are in its `licenses` folder.

The original qBittorrent README is kept in [README.qBittorrent.md](README.qBittorrent.md).
