# Publishing Mankka's Edition — notes and checklist

Private notes for Mankka. Nothing has been published; everything is local. The official
qBittorrent project must **never** receive this code (no pull requests, issues or
patches) — publishing means your **own** repository only.

This is practical guidance, not legal advice. Status as of 2026-10-18.

---

## 1. The licence (GPL) — what you must do

qBittorrent's source code is GPLv2-or-later; the compiled program is GPLv3-or-later
(some images are GPLv3), with an extra permission for linking OpenSSL.

- [x] `COPYING`, `COPYING.GPLv2`, `COPYING.GPLv3`, `AUTHORS` and the copyright headers in
      every file are kept. Don't remove anyone's name.
- [x] Your changes are GPL too; the README says so. Don't add restrictions (e.g. "no
      commercial use").
- [x] It's clear that the files were changed, and when: the git history,
      `LOCAL_CHANGES.md` and the README.
- [x] The release zip (`dist/mankkas-edition/package-release.ps1`) includes the licences
      of the bundled parts — Qt (LGPLv3), OpenSSL (Apache 2.0), Boost (BSL-1.0),
      libtorrent (BSD), zlib — plus COPYING and AUTHORS, in its `licenses` folder. The
      complete source must stay available in the repository for as long as the zip is
      offered.
- [x] The DB-IP credit is in the About dialog, the README and `licenses/README.txt`.

## 2. Name and identity

- [x] The name is "qBittorrent vX.Y.Z Mankka's Edition", and About says "Modifications ©
      2026 Mankka" — you own the copyright to your changes, not the whole program.
- [x] The README and About say it is **unofficial and not affiliated with the qBittorrent
      project**.
- [ ] Don't use qBittorrent's logo as your own project's logo or GitHub avatar.
- [ ] I don't know whether "qBittorrent" is a registered trademark. Other forks use the
      name (e.g. "qBittorrent Enhanced Edition"), but if the project ever asks you to
      rename, be ready to pick your own name.

## 3. Privacy

- [x] The author name in your commits is "Mankka".
- [x] Your commits use your GitHub private address
      (`340949780+mankka-beep@users.noreply.github.com`) instead of your personal email.
      Keep "Keep my email addresses private" turned on in GitHub → Settings → Emails.
- [ ] `LOCAL_CHANGES.md` mentions local paths such as `C:\qbt-deps` and `%APPDATA%`.
      Harmless, but you may want to tidy it.

## 4. Trackers and fairness to others

- [x] The README warns that some **private trackers forbid modified clients** (the edition
      tells trackers it is qBittorrent 5.4.0).
- [x] The README and user guide say that bans only apply to seeding torrents.

## 5. Code written with AI help

- [x] The README mentions that much of the edition was written with Claude; the commits
      say so too ("Co-Authored-By: Claude").
- [ ] You are responsible for the code you publish. Test each release yourself before
      uploading it.
- [x] Never send anything to the official project (qBittorrent's `AGENTS.md` also asks for
      no AI-written contributions).

## 6. The repository and releases

- [x] `README.md` is the edition's front page; qBittorrent's original is
      `README.qBittorrent.md`.
- [x] Support, security, contributing pages and the issue/pull request forms are the
      edition's own and ask people not to report to the official project.
- [x] qBittorrent's GitHub Actions workflows and Dependabot are removed (only the
      pre-commit helper scripts are kept).
- [x] Building: `dist/mankkas-edition/build-windows.ps1` and `docs/mankkas-edition/BUILDING.md`.
- [x] Release zip: `dist/mankkas-edition/package-release.ps1` →
      `release\qBittorrent-Mankkas-Edition-vX.Y.Z-win64.zip`, tested to start on its own.
- [ ] Create your own repository on GitHub (e.g. `mankka-beep/qBittorrent-Mankkas-Edition`)
      — **not** a fork-contribution to the official project.
- [ ] Decide how much support you want to give (GitHub Issues can be turned off).
- [ ] Keep `master` as the official code and your changes on `my-changes`, so you can move to
      new qBittorrent releases later (that would be version v2.0.0).
- [ ] Each release: raise the version number, update `LOCAL_CHANGES.md` and the user guide,
      build, run `package-release.ps1`, tag the commit (e.g. `v1.1.0`) and upload the zip.

## 7. Your personal story

- [x] Written on 2026-10-18: the "Why I made this version?" section in `README.md`.

The questions that were used as a starting point, in case you want to add more later:

- What were you missing in normal qBittorrent? What annoyed you?
- What made you start changing it, and when?
- What does the edition do better for you in daily use?
- Who might find it useful, and who should rather stay with the official version?
- What do you hope to add or improve next?
- How did you build it (e.g. together with an AI assistant)?
