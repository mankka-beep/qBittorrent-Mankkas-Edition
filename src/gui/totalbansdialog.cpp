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

#include "totalbansdialog.h"

#include <algorithm>

#include <QDialogButtonBox>
#include <QHash>
#include <QHeaderView>
#include <QHostAddress>
#include <QLabel>
#include <QList>
#include <QLocale>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "base/bittorrent/session.h"
#include "base/global.h"
#include "base/net/geoipmanager.h"
#include "uithememanager.h"

#define SETTINGS_KEY(name) u"TotalBansDialog/" name

TotalBansDialog::TotalBansDialog(QWidget *parent)
    : QDialog(parent)
    , m_storeDialogSize {SETTINGS_KEY(u"Size"_s)}
{
    setWindowTitle(tr("Total bans"));
    setAttribute(Qt::WA_DeleteOnClose);

    // Count the manually banned IP addresses per country
    const QStringList bannedIPs = BitTorrent::Session::instance()->bannedIPs();
    QList<QHostAddress> addresses;
    addresses.reserve(bannedIPs.size());
    for (const QString &ip : bannedIPs)
        addresses.append(QHostAddress(ip));

    const QStringList countries = Net::GeoIPManager::instance()->lookupCountries(addresses);
    QHash<QString, int> bansPerCountry;
    for (const QString &country : countries)
        ++bansPerCountry[country];  // an empty code collects the addresses without a known country

    struct CountryBans
    {
        QString code;
        QString name;
        int bans = 0;
    };

    QList<CountryBans> rows;
    rows.reserve(bansPerCountry.size());
    for (auto iter = bansPerCountry.cbegin(); iter != bansPerCountry.cend(); ++iter)
        rows.append({iter.key(), Net::GeoIPManager::CountryName(iter.key()), iter.value()});

    // Most bans first; countries with as many bans in alphabetical order
    std::sort(rows.begin(), rows.end(), [](const CountryBans &left, const CountryBans &right)
    {
        if (left.bans != right.bans)
            return (left.bans > right.bans);
        return (QString::localeAwareCompare(left.name, right.name) < 0);
    });

    const QLocale locale;
    auto *list = new QTreeWidget(this);
    list->setColumnCount(2);
    list->setHeaderLabels({tr("Country"), tr("Bans")});
    list->setRootIsDecorated(false);
    list->setUniformRowHeights(true);
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    list->header()->setStretchLastSection(false);
    list->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    list->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    list->headerItem()->setTextAlignment(1, (Qt::AlignRight | Qt::AlignVCenter));
    for (const CountryBans &row : asConst(rows))
    {
        auto *item = new QTreeWidgetItem(list);
        item->setIcon(0, UIThemeManager::instance()->getFlagIcon(row.code));
        item->setText(0, row.name);
        item->setText(1, locale.toString(row.bans));
        item->setTextAlignment(1, (Qt::AlignRight | Qt::AlignVCenter));
    }

    auto *totalLabel = new QLabel(tr("Total: %1 bans in %2 countries")
            .arg(locale.toString(bannedIPs.size()), locale.toString(bansPerCountry.size() - (bansPerCountry.contains({}) ? 1 : 0)))
            , this);
    QFont boldFont = totalLabel->font();
    boldFont.setBold(true);
    totalLabel->setFont(boldFont);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(list);
    layout->addWidget(totalLabel);
    layout->addWidget(buttonBox);

    resize(400, 500);
    if (const QSize dialogSize = m_storeDialogSize; dialogSize.isValid())
        resize(dialogSize);
}

TotalBansDialog::~TotalBansDialog()
{
    m_storeDialogSize = size();
}
