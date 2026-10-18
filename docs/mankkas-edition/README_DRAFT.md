# qBittorrent Mankka's Edition

An **unofficial**, modified version of [qBittorrent](https://www.qbittorrent.org/) for
Windows with extra tools for blocking peers by country, faster banning and ban
statistics.

> **This is not the official qBittorrent.** It is not affiliated with or endorsed by the
> qBittorrent project. Please don't report problems with this edition to them.

Current version: **v1.1.0**, based on qBittorrent v5.4.0alpha1.

## Why I made this

<!-- Write your own story here, in your own words. Some ideas:
     - what you were missing in normal qBittorrent
     - what made you start changing it
     - what it does better for you in daily use
     - who you think it is for
     - what you'd like to add next
-->

*(Mankka's personal story goes here.)*

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

See the [user guide](USER_GUIDE.md) for details, and `LOCAL_CHANGES.md` for every change.

## Things to know

- To trackers and peers the program identifies itself as qBittorrent 5.4.0. **Some
  private trackers don't allow modified clients** — check your trackers' rules first.
- Country detection uses the free IP to Country Lite database by
  [DB-IP](https://db-ip.com/) (CC BY 4.0).
- Much of this edition was written with the help of Claude, an AI assistant, and
  reviewed and tested by me.
- Provided as is, without warranty.

## Licence

Like qBittorrent, this edition is free software under the GNU General Public License:
the source code is GPLv2 or later, the compiled program GPLv3 or later, with an
exception for linking OpenSSL. See `COPYING`. qBittorrent is © its contributors (see
`AUTHORS`); the modifications are © 2026 Mankka.
