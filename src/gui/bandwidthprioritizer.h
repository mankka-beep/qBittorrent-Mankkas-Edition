/*
 * Bittorrent Client using Qt and libtorrent.
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

#pragma once

#include <optional>

#include <QtTypes>
#include <QElapsedTimer>
#include <QHash>
#include <QList>
#include <QObject>
#include <QSet>
#include <QStringList>

#include "base/bittorrent/infohash.h"
#include "base/settingvalue.h"

class QTimer;

namespace BitTorrent
{
    class Torrent;
}

// Gives bandwidth first to the torrents checked in the "#" column of the transfer list.
//
// Every second, separately for download and upload, the available bandwidth (the global
// speed limit, or the highest total speed measured when there is no limit) is shared:
// checked torrents get it first, equally among them, and what one of them cannot use goes
// to the other checked torrents. The other running torrents share what is left. This is
// done by setting temporary per-torrent speed limits; each torrent's own limits are backed
// up, still respected as an upper bound and restored as soon as they are no longer needed.
class BandwidthPrioritizer final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BandwidthPrioritizer)

public:
    explicit BandwidthPrioritizer(QObject *parent = nullptr);
    ~BandwidthPrioritizer() override;

    static BandwidthPrioritizer *instance();

    bool isPrioritized(const BitTorrent::Torrent *torrent) const;
    void setPrioritized(BitTorrent::Torrent *torrent, bool prioritized);

    // Stops managing speed limits and gives every torrent its own limits back
    void restoreAllLimits();

signals:
    void prioritizedChanged(BitTorrent::Torrent *torrent);

private:
    enum class Direction
    {
        Download,
        Upload
    };

    struct ManagedLimits
    {
        // Torrent's own limit, saved while its limit is set by us (0 means unlimited)
        std::optional<int> originalDownloadLimit;
        std::optional<int> originalUploadLimit;
        // When we last changed the limit (milliseconds of m_clock)
        qint64 lastDownloadLimitChange = 0;
        qint64 lastUploadLimitChange = 0;
    };

    void update();
    void updateDirection(Direction direction, const QList<BitTorrent::Torrent *> &torrents);
    qint64 capacity(Direction direction) const;
    void applyLimit(BitTorrent::Torrent *torrent, Direction direction, int limit);
    void releaseLimit(BitTorrent::Torrent *torrent, Direction direction);
    void handleTorrentAboutToBeRemoved(BitTorrent::Torrent *torrent);
    void loadSettings();
    void storePrioritized();
    void storeOriginalLimits();

    static BandwidthPrioritizer *m_instance;

    QTimer *m_updateTimer = nullptr;
    QElapsedTimer m_clock;
    QSet<BitTorrent::TorrentID> m_prioritized;
    QHash<BitTorrent::TorrentID, ManagedLimits> m_managedLimits;
    bool m_originalLimitsChanged = false;
    qreal m_peakDownloadRate = 0;
    qreal m_peakUploadRate = 0;
    int m_ticksSincePeakStored = 0;

    SettingValue<QStringList> m_storedPrioritized;
    SettingValue<QStringList> m_storedOriginalLimits;
    SettingValue<qint64> m_storedPeakDownloadRate;
    SettingValue<qint64> m_storedPeakUploadRate;
};
