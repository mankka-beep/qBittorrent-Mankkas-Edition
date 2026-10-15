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

#include "bandwidthprioritizer.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <numeric>

#include <QTimer>

#include "base/bittorrent/session.h"
#include "base/bittorrent/sessionstatus.h"
#include "base/bittorrent/torrent.h"
#include "base/global.h"

using namespace std::chrono_literals;

namespace
{
#define SETTINGS_KEY(name) (u"GUI/BandwidthPriority/" name)

    const auto UPDATE_INTERVAL = 1s;

    // How much a torrent is allowed to grow in one update: its current rate times
    // GROWTH_FACTOR, plus PROBE_SHARE of the capacity (but at least MIN_PROBE)
    const qreal GROWTH_FACTOR = 1.5;
    const qreal PROBE_SHARE = 0.05;
    const qint64 MIN_PROBE = 10 * 1024;

    // Lowest speed limit set on a torrent (0 would mean "unlimited")
    const int MIN_LIMIT = 1024;

    // The measured peak speed slowly fades (half-life of one hour) so that the capacity
    // follows a slower connection too
    const qreal PEAK_DECAY = std::pow(0.5, (1.0 / 3600));

    // Every limit change makes the torrent save its resume data, so a limit is changed only
    // when it differs by more than LIMIT_CHANGE_THRESHOLD, and at most once per
    // MIN_LIMIT_CHANGE_INTERVAL unless it differs by more than BIG_LIMIT_CHANGE_THRESHOLD
    // (or switches to or from unlimited)
    const qreal LIMIT_CHANGE_THRESHOLD = 0.1;
    const qreal BIG_LIMIT_CHANGE_THRESHOLD = 0.5;
    const qint64 MIN_LIMIT_CHANGE_INTERVAL = 5000;

    // Shares `budget` between consumers asking for `demands`: nobody gets more than it asks
    // for, and what one does not need is shared equally by the others
    QList<qint64> shareBandwidth(const qint64 budget, const QList<qint64> &demands)
    {
        QList<qsizetype> order(demands.size());
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&demands](const qsizetype left, const qsizetype right)
        {
            return demands[left] < demands[right];
        });

        QList<qint64> shares(demands.size(), 0);
        qint64 remaining = budget;
        qsizetype consumersLeft = demands.size();
        for (const qsizetype i : asConst(order))
        {
            const qint64 share = std::min(demands[i], (remaining / consumersLeft));
            shares[i] = share;
            remaining -= share;
            --consumersLeft;
        }
        return shares;
    }

    bool needsBandwidth(const BitTorrent::Torrent *torrent, const bool download)
    {
        if (torrent->isStopped() || torrent->isErrored())
            return false;
        // A finished torrent downloads nothing
        return !download || !torrent->isFinished();
    }

    int limitOf(const BitTorrent::Torrent *torrent, const bool download)
    {
        return download ? torrent->downloadLimit() : torrent->uploadLimit();
    }

    void setLimitOf(BitTorrent::Torrent *torrent, const bool download, const int limit)
    {
        if (download)
            torrent->setDownloadLimit(limit);
        else
            torrent->setUploadLimit(limit);
    }

    bool isLimitChangeNeeded(const int currentLimit, const int newLimit, const qint64 msSinceLastChange)
    {
        if (currentLimit == newLimit)
            return false;
        if ((currentLimit <= 0) || (newLimit <= 0))
            return true;

        const qreal change = std::abs(static_cast<qreal>(newLimit) - currentLimit) / currentLimit;
        if (change > BIG_LIMIT_CHANGE_THRESHOLD)
            return true;
        return (change > LIMIT_CHANGE_THRESHOLD) && (msSinceLastChange >= MIN_LIMIT_CHANGE_INTERVAL);
    }
}

BandwidthPrioritizer *BandwidthPrioritizer::m_instance = nullptr;

BandwidthPrioritizer::BandwidthPrioritizer(QObject *parent)
    : QObject(parent)
    , m_updateTimer {new QTimer(this)}
    , m_storedPrioritized {SETTINGS_KEY(u"Torrents"_s)}
    , m_storedOriginalLimits {SETTINGS_KEY(u"OriginalLimits"_s)}
    , m_storedPeakDownloadRate {SETTINGS_KEY(u"PeakDownloadRate"_s)}
    , m_storedPeakUploadRate {SETTINGS_KEY(u"PeakUploadRate"_s)}
{
    Q_ASSERT(!m_instance);
    m_instance = this;

    m_clock.start();
    loadSettings();

    connect(BitTorrent::Session::instance(), &BitTorrent::Session::torrentAboutToBeRemoved
            , this, &BandwidthPrioritizer::handleTorrentAboutToBeRemoved);
    connect(m_updateTimer, &QTimer::timeout, this, &BandwidthPrioritizer::update);
    m_updateTimer->start(UPDATE_INTERVAL);
}

BandwidthPrioritizer::~BandwidthPrioritizer()
{
    m_instance = nullptr;
}

BandwidthPrioritizer *BandwidthPrioritizer::instance()
{
    return m_instance;
}

bool BandwidthPrioritizer::isPrioritized(const BitTorrent::Torrent *torrent) const
{
    return m_prioritized.contains(torrent->id());
}

void BandwidthPrioritizer::setPrioritized(BitTorrent::Torrent *torrent, const bool prioritized)
{
    if (prioritized == isPrioritized(torrent))
        return;

    if (prioritized)
        m_prioritized.insert(torrent->id());
    else
        m_prioritized.remove(torrent->id());

    storePrioritized();
    emit prioritizedChanged(torrent);

    // Apply the change right away instead of on the next update
    update();
}

void BandwidthPrioritizer::restoreAllLimits()
{
    m_updateTimer->stop();

    const auto *session = BitTorrent::Session::instance();
    for (auto iter = m_managedLimits.cbegin(); iter != m_managedLimits.cend(); ++iter)
    {
        BitTorrent::Torrent *torrent = session->getTorrent(iter.key());
        if (!torrent)
            continue;

        if (iter->originalDownloadLimit)
            torrent->setDownloadLimit(*iter->originalDownloadLimit);
        if (iter->originalUploadLimit)
            torrent->setUploadLimit(*iter->originalUploadLimit);
    }

    m_managedLimits.clear();
    storeOriginalLimits();

    m_storedPeakDownloadRate = static_cast<qint64>(m_peakDownloadRate);
    m_storedPeakUploadRate = static_cast<qint64>(m_peakUploadRate);
}

void BandwidthPrioritizer::update()
{
    const auto *session = BitTorrent::Session::instance();
    const BitTorrent::SessionStatus &status = session->status();
    m_peakDownloadRate = std::max<qreal>(status.payloadDownloadRate, (m_peakDownloadRate * PEAK_DECAY));
    m_peakUploadRate = std::max<qreal>(status.payloadUploadRate, (m_peakUploadRate * PEAK_DECAY));

    const QList<BitTorrent::Torrent *> torrents = session->torrents();
    updateDirection(Direction::Download, torrents);
    updateDirection(Direction::Upload, torrents);

    if (m_originalLimitsChanged)
        storeOriginalLimits();

    if (++m_ticksSincePeakStored >= 60)
    {
        m_ticksSincePeakStored = 0;
        m_storedPeakDownloadRate = static_cast<qint64>(m_peakDownloadRate);
        m_storedPeakUploadRate = static_cast<qint64>(m_peakUploadRate);
    }
}

void BandwidthPrioritizer::updateDirection(const Direction direction, const QList<BitTorrent::Torrent *> &torrents)
{
    const bool download = (direction == Direction::Download);
    const qint64 totalCapacity = capacity(direction);

    QList<BitTorrent::Torrent *> prioritizedTorrents;
    QList<BitTorrent::Torrent *> otherTorrents;
    for (BitTorrent::Torrent *torrent : torrents)
    {
        if (!needsBandwidth(torrent, download))
            releaseLimit(torrent, direction);
        else if (m_prioritized.contains(torrent->id()))
            prioritizedTorrents.append(torrent);
        else
            otherTorrents.append(torrent);
    }

    // Nothing to prioritize: every torrent works with its own limits as usual
    if (prioritizedTorrents.isEmpty() || (totalCapacity <= 0))
    {
        for (BitTorrent::Torrent *torrent : asConst(prioritizedTorrents))
            releaseLimit(torrent, direction);
        for (BitTorrent::Torrent *torrent : asConst(otherTorrents))
            releaseLimit(torrent, direction);
        return;
    }

    const qint64 probe = std::max(static_cast<qint64>(totalCapacity * PROBE_SHARE), MIN_PROBE);
    const auto demandsOf = [download, probe](const QList<BitTorrent::Torrent *> &list)
    {
        QList<qint64> demands;
        demands.reserve(list.size());
        for (const BitTorrent::Torrent *torrent : list)
        {
            const int rate = download ? torrent->downloadPayloadRate() : torrent->uploadPayloadRate();
            demands.append(static_cast<qint64>(rate * GROWTH_FACTOR) + probe);
        }
        return demands;
    };

    // Prioritized torrents share the whole capacity among themselves
    const QList<qint64> prioritizedShares = shareBandwidth(totalCapacity, demandsOf(prioritizedTorrents));
    const qint64 reserved = std::accumulate(prioritizedShares.cbegin(), prioritizedShares.cend(), qint64 {0});
    const qint64 equalShare = totalCapacity / prioritizedTorrents.size();
    for (qsizetype i = 0; i < prioritizedTorrents.size(); ++i)
    {
        // A single prioritized torrent may take everything it can. With more of them, each one
        // can always reach its equal share, and more when the others do not need theirs.
        const qint64 limit = (prioritizedTorrents.size() == 1)
                ? 0 : std::max(prioritizedShares[i], equalShare);
        applyLimit(prioritizedTorrents[i], direction, static_cast<int>(std::min<qint64>(limit, std::numeric_limits<int>::max())));
    }

    // The other torrents share what the prioritized ones leave
    const qint64 leftover = std::max<qint64>((totalCapacity - reserved), 0);
    const QList<qint64> otherShares = shareBandwidth(leftover, demandsOf(otherTorrents));
    for (qsizetype i = 0; i < otherTorrents.size(); ++i)
    {
        const qint64 limit = std::max<qint64>(otherShares[i], MIN_LIMIT);
        applyLimit(otherTorrents[i], direction, static_cast<int>(std::min<qint64>(limit, std::numeric_limits<int>::max())));
    }
}

qint64 BandwidthPrioritizer::capacity(const Direction direction) const
{
    const auto *session = BitTorrent::Session::instance();
    const bool download = (direction == Direction::Download);

    // Global speed limit (the alternative one when it is enabled), if one is set
    const int globalLimit = download ? session->downloadSpeedLimit() : session->uploadSpeedLimit();
    if (globalLimit > 0)
        return globalLimit;

    return static_cast<qint64>(download ? m_peakDownloadRate : m_peakUploadRate);
}

void BandwidthPrioritizer::applyLimit(BitTorrent::Torrent *torrent, const Direction direction, const int limit)
{
    const bool download = (direction == Direction::Download);
    ManagedLimits &managed = m_managedLimits[torrent->id()];
    std::optional<int> &originalLimit = download ? managed.originalDownloadLimit : managed.originalUploadLimit;
    if (!originalLimit)
    {
        originalLimit = limitOf(torrent, download);
        m_originalLimitsChanged = true;
    }

    // The torrent's own limit stays an upper bound
    int newLimit = limit;
    if (*originalLimit > 0)
        newLimit = (limit > 0) ? std::min(limit, *originalLimit) : *originalLimit;

    qint64 &lastLimitChange = download ? managed.lastDownloadLimitChange : managed.lastUploadLimitChange;
    const qint64 now = m_clock.elapsed();
    if (isLimitChangeNeeded(limitOf(torrent, download), newLimit, (now - lastLimitChange)))
    {
        setLimitOf(torrent, download, newLimit);
        lastLimitChange = now;
    }
}

void BandwidthPrioritizer::releaseLimit(BitTorrent::Torrent *torrent, const Direction direction)
{
    const auto iter = m_managedLimits.find(torrent->id());
    if (iter == m_managedLimits.end())
        return;

    const bool download = (direction == Direction::Download);
    std::optional<int> &originalLimit = download ? iter->originalDownloadLimit : iter->originalUploadLimit;
    if (!originalLimit)
        return;

    setLimitOf(torrent, download, *originalLimit);
    originalLimit.reset();
    if (!iter->originalDownloadLimit && !iter->originalUploadLimit)
        m_managedLimits.erase(iter);
    m_originalLimitsChanged = true;
}

void BandwidthPrioritizer::handleTorrentAboutToBeRemoved(BitTorrent::Torrent *torrent)
{
    if (m_prioritized.remove(torrent->id()))
        storePrioritized();
    if (m_managedLimits.remove(torrent->id()))
        storeOriginalLimits();
}

void BandwidthPrioritizer::loadSettings()
{
    for (const QString &idString : asConst(m_storedPrioritized.get()))
    {
        const auto id = BitTorrent::TorrentID::fromString(idString);
        if (id.isValid())
            m_prioritized.insert(id);
    }

    // Limits backed up by a previous run that did not get to restore them (e.g. a crash) are
    // still treated as the torrents' own limits, so they get restored when no longer needed
    for (const QString &entry : asConst(m_storedOriginalLimits.get()))
    {
        const QStringList fields = entry.split(u'|');
        if (fields.size() != 3)
            continue;

        const auto id = BitTorrent::TorrentID::fromString(fields[0]);
        if (!id.isValid())
            continue;

        ManagedLimits managed;
        bool ok = false;
        if (const int value = fields[1].toInt(&ok); ok)
            managed.originalDownloadLimit = value;
        if (const int value = fields[2].toInt(&ok); ok)
            managed.originalUploadLimit = value;
        if (managed.originalDownloadLimit || managed.originalUploadLimit)
            m_managedLimits.insert(id, managed);
    }

    m_peakDownloadRate = m_storedPeakDownloadRate.get(0);
    m_peakUploadRate = m_storedPeakUploadRate.get(0);
}

void BandwidthPrioritizer::storePrioritized()
{
    QStringList ids;
    ids.reserve(m_prioritized.size());
    for (const BitTorrent::TorrentID &id : asConst(m_prioritized))
        ids.append(id.toString());
    m_storedPrioritized = ids;
}

void BandwidthPrioritizer::storeOriginalLimits()
{
    // One entry per torrent: "<torrent ID>|<download limit>|<upload limit>", where a limit
    // is empty when that direction is not managed
    const auto limitString = [](const std::optional<int> &limit)
    {
        return limit ? QString::number(*limit) : QString();
    };

    QStringList entries;
    entries.reserve(m_managedLimits.size());
    for (auto iter = m_managedLimits.cbegin(); iter != m_managedLimits.cend(); ++iter)
    {
        entries.append(iter.key().toString() + u'|' + limitString(iter->originalDownloadLimit)
                + u'|' + limitString(iter->originalUploadLimit));
    }
    m_storedOriginalLimits = entries;
    m_originalLimitsChanged = false;
}
