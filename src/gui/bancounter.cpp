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

#include "bancounter.h"

#include <QDate>
#include <QString>

#include "base/global.h"
#include "base/settingvalue.h"

namespace
{
    // The date (ISO 8601) the count belongs to, and the count itself
    SettingValue<QString> storedDate()
    {
        return SettingValue<QString> {u"GUI/BanCountries/TodayDate"_s};
    }

    SettingValue<int> storedCount()
    {
        return SettingValue<int> {u"GUI/BanCountries/TodayCount"_s};
    }

    QString today()
    {
        return QDate::currentDate().toString(Qt::ISODate);
    }
}

void BanCounter::addBanned(const int count)
{
    if (count <= 0)
        return;

    const int total = bannedToday() + count;
    storedDate() = today();
    storedCount() = total;
}

int BanCounter::bannedToday()
{
    if (storedDate().get() != today())
        return 0;
    return storedCount().get(0);
}
