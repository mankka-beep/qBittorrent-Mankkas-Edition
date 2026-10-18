# qBittorrent Mankka's Edition

An **unofficial**, modified version of [qBittorrent](https://www.qbittorrent.org/) for
Windows with extra tools for blocking peers by country, faster banning and ban
statistics.

> **This is not the official qBittorrent.** It is not affiliated with or endorsed by the
> qBittorrent project. Please don't report problems with this edition to them.

Current version: **v1.1.0**, based on qBittorrent v5.4.0alpha1.

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
