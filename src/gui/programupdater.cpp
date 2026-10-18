/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2021  Mike Tzou (Chocobo1)
 * Copyright (C) 2010  Christophe Dumez <chris@qbittorrent.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include "programupdater.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonValue>
#include <QUrl>

#include "base/global.h"
#include "base/logger.h"
#include "base/net/downloadmanager.h"
#include "base/preferences.h"
#include "base/version.h"

namespace
{
    // GitHub's latest published (non-draft, non-prerelease) release of the edition
    const QString LATEST_RELEASE_API_URL = u"https://api.github.com/repos/" QBT_MOD_GITHUB_REPO "/releases/latest"_s;
    const QString RELEASES_PAGE_URL = u"" QBT_MOD_GITHUB_URL "/releases"_s;

    // "v1.2.3" -> 1.2.3
    ProgramUpdater::Version versionFromTag(const QString &tag)
    {
        const QStringView versionString = (tag.startsWith(u'v') || tag.startsWith(u'V')) ? QStringView(tag).mid(1) : QStringView(tag);
        return ProgramUpdater::Version::fromString(versionString);
    }
}

void ProgramUpdater::checkForUpdates()
{
    m_newVersion = {};
    m_releaseUrl = RELEASES_PAGE_URL;

    // GitHub's API requires a User-Agent that identifies the program
    const auto request = Net::DownloadRequest(LATEST_RELEASE_API_URL)
            .userAgent(u"qBittorrent-Mankkas-Edition/" QBT_MOD_VERSION " (" QBT_MOD_GITHUB_URL ")"_s);
    const bool useProxy = Preferences::instance()->useProxyForGeneralPurposes();
    Net::DownloadManager::instance()->download(request, useProxy, this, &ProgramUpdater::handleLatestReleaseDownloaded);
}

ProgramUpdater::Version ProgramUpdater::getNewVersion() const
{
    return m_newVersion;
}

QString ProgramUpdater::releaseUrl() const
{
    return m_releaseUrl;
}

void ProgramUpdater::handleLatestReleaseDownloaded(const Net::DownloadResult &result)
{
    if (result.status != Net::DownloadStatus::Success)
    {
        // Also happens while the repository has no releases yet (GitHub answers "404 Not Found")
        LogMsg(tr("Failed to download the program update info. URL: \"%1\". Error: \"%2\"").arg(result.url, result.errorString), Log::WARNING);
        emit updateCheckFinished();
        return;
    }

    const QJsonDocument json = QJsonDocument::fromJson(result.data);
    const Version latestVersion = versionFromTag(json[u"tag_name"_s].toString());
    const Version currentVersion = versionFromTag(QStringLiteral(QBT_MOD_VERSION));
    if (latestVersion.isValid() && currentVersion.isValid() && (latestVersion > currentVersion))
    {
        m_newVersion = latestVersion;
        if (const QString releasePage = json[u"html_url"_s].toString(); releasePage.startsWith(u"" QBT_MOD_GITHUB_URL "/"_s))
            m_releaseUrl = releasePage;
    }

    emit updateCheckFinished();
}

bool ProgramUpdater::updateProgram() const
{
    return QDesktopServices::openUrl(QUrl(m_releaseUrl));
}
