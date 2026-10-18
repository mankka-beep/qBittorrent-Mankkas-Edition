# Publishing Mankka's Edition — notes and checklist

Private notes for Mankka. Nothing has been published; everything is local. The official
qBittorrent project must **never** receive this code (no pull requests, issues or
patches) — publishing means your **own** repository only.

This is practical guidance, not legal advice.

---

## 1. The licence (GPL) — what you must do

qBittorrent's source code is GPLv2-or-later; the compiled program is GPLv3-or-later
(some images are GPLv3), with an extra permission for linking OpenSSL.

- [ ] Keep `COPYING`, `COPYING.GPLv2`, `COPYING.GPLv3`, `AUTHORS` and the copyright
      headers in every file. Don't remove anyone's name.
- [ ] Your changes are GPL too. Don't add restrictions (e.g. "no commercial use").
- [ ] Say clearly that the files were changed, and when. The git history and
      `LOCAL_CHANGES.md` do this; the README should mention it as well.
- [ ] If you publish the `.exe` (e.g. as a GitHub Release): the complete source must be
      available (the repository), and the release must include the licences of the
      bundled libraries: Qt (LGPLv3), OpenSSL (Apache 2.0), Boost (BSL-1.0),
      libtorrent (BSD) and zlib.
- [ ] Keep the DB-IP credit (the About dialog has it). The flag images (flag-icons, MIT)
      are already included by qBittorrent.

## 2. Name and identity

- [x] The name is "qBittorrent vX.Y.Z Mankka's Edition", and About says "Modifications ©
      2026 Mankka" — you own the copyright to your changes, not the whole program.
- [ ] The README must say it is **unofficial and not affiliated with the qBittorrent
      project**. (The About dialog already does.)
- [ ] Don't use qBittorrent's logo as your own project's logo or GitHub avatar.
- [ ] I don't know whether "qBittorrent" is a registered trademark. Other forks use the
      name (e.g. "qBittorrent Enhanced Edition"), but if the project ever asks you to
      rename, be ready to pick your own name.

## 3. Privacy

- [x] The author name in your commits is "Mankka" (fixed on 2026-10-18).
- [x] Your commits use your GitHub private address
      (`340949780+mankka-beep@users.noreply.github.com`) instead of your personal email
      (changed on 2026-10-18). Keep "Keep my email addresses private" turned on in
      GitHub → Settings → Emails.
- [ ] `LOCAL_CHANGES.md` mentions local paths such as `C:\qbt-deps` and `%APPDATA%`. Harmless,
      but you may want to tidy it.

## 4. Trackers and fairness to others

- [ ] The edition tells trackers it is qBittorrent 5.4.0. Some **private trackers forbid
      modified clients**, even ones that look official. Mention this in the README so
      users don't break their trackers' rules.
- [ ] Bans only apply to seeding torrents, so downloads still use banned peers. That is
      your choice to make, but say so in the README.

## 5. Code written with AI help

- [ ] Much of this edition was written with Claude (an AI assistant). The commits say so
      ("Co-Authored-By: Claude"). It's honest to mention it in the README too.
- [ ] You are responsible for the code you publish. Test each release yourself before
      uploading it.
- [ ] qBittorrent's own rules (`AGENTS.md`) ask that AI-written contributions are not sent
      to them — another reason never to open pull requests or issues upstream.

## 6. Practical things for a GitHub release

- [ ] Use your own repository (e.g. `github.com/<your-name>/qbittorrent-mankkas-edition`),
      not a pull request or fork-contribution to the official project.
- [ ] Decide whether to publish only the source, or also a ready-made `.exe`. A Windows
      build needs the Qt/OpenSSL/Boost/libtorrent DLLs next to it (the build folder has
      them), plus the licence files.
- [ ] Document how to build it. The local build script `C:\qbt-deps\build-qbt.ps1`
      contains your own folder paths; a cleaned-up copy can go into the repository.
- [ ] Decide how much support you want to give: GitHub Issues can be turned off, or you
      can say "provided as is".
- [ ] Keep `master` as the official code and your changes on top, so you can move to new
      qBittorrent releases later (that would be version v2.0.0 under the numbering rules).
- [ ] Each release: raise the version number, update `LOCAL_CHANGES.md` and the user guide,
      and tag the commit (e.g. `v1.1.0`).

## 7. Your personal story

- [x] Written on 2026-10-18: the "Why I made this version?" section in `README_DRAFT.md`.

The questions that were used as a starting point, in case you want to add more later:

- What were you missing in normal qBittorrent? What annoyed you?
- What made you start changing it, and when?
- What does the edition do better for you in daily use?
- Who might find it useful, and who should rather stay with the official version?
- What do you hope to add or improve next?
- How did you build it (e.g. together with an AI assistant)?
