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
#include <chrono>

#include <QDialogButtonBox>
#include <QHash>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QHostAddress>
#include <QLabel>
#include <QList>
#include <QLocale>
#include <QPushButton>
#include <QSet>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "base/bittorrent/session.h"
#include "base/global.h"
#include "base/logger.h"
#include "base/net/geoipmanager.h"
#include "bancounter.h"
#include "uithememanager.h"

using namespace std::chrono_literals;

#define SETTINGS_KEY(name) u"TotalBansDialog/" name

namespace
{
    const int COUNTRY_CODE_ROLE = Qt::UserRole;

    // The country of each banned IP address, in the order of the list (empty when unknown)
    QStringList countriesOf(const QStringList &bannedIPs)
    {
        QList<QHostAddress> addresses;
        addresses.reserve(bannedIPs.size());
        for (const QString &ip : bannedIPs)
            addresses.append(QHostAddress(ip));
        return Net::GeoIPManager::instance()->lookupCountries(addresses);
    }
}

TotalBansDialog::TotalBansDialog(QWidget *parent)
    : QDialog(parent)
    , m_list {new QTreeWidget(this)}
    , m_totalLabel {new QLabel(this)}
    , m_blockedTodayLabel {new QLabel(this)}
    , m_unbanButton {new QPushButton(tr("Unban"), this)}
    , m_storeDialogSize {SETTINGS_KEY(u"Size"_s)}
{
    setWindowTitle(tr("Total bans"));
    setAttribute(Qt::WA_DeleteOnClose);

    m_list->setColumnCount(2);
    m_list->setHeaderLabels({tr("Country"), tr("Bans")});
    m_list->setRootIsDecorated(false);
    m_list->setUniformRowHeights(true);
    m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_list->header()->setStretchLastSection(false);
    m_list->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_list->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_list->headerItem()->setTextAlignment(1, (Qt::AlignRight | Qt::AlignVCenter));

    QFont boldFont = m_totalLabel->font();
    boldFont.setBold(true);
    m_totalLabel->setFont(boldFont);
    m_blockedTodayLabel->setFont(boldFont);
    m_blockedTodayLabel->setToolTip(tr("Connections the IP filter (the banned IP list) refused today"));

    // The number of blocks keeps changing, so it is refreshed while the window is open
    auto *blockedTodayTimer = new QTimer(this);
    connect(blockedTodayTimer, &QTimer::timeout, this, &TotalBansDialog::updateBlockedTodayLabel);
    blockedTodayTimer->start(2s);
    updateBlockedTodayLabel();

    m_unbanButton->setToolTip(tr("Remove all banned IP addresses of the selected countries from the banned IP list"));
    m_unbanButton->setEnabled(false);
    connect(m_unbanButton, &QPushButton::clicked, this, &TotalBansDialog::unbanSelectedCountries);
    connect(m_list, &QTreeWidget::itemSelectionChanged, this, [this]
    {
        m_unbanButton->setEnabled(!m_list->selectedItems().isEmpty());
    });

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // "Unban" in the lower left corner, "Close" on the right
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_unbanButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonBox);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_list);
    layout->addWidget(m_totalLabel);
    layout->addWidget(m_blockedTodayLabel);
    layout->addLayout(buttonLayout);

    populate();

    resize(400, 500);
    if (const QSize dialogSize = m_storeDialogSize; dialogSize.isValid())
        resize(dialogSize);
}

TotalBansDialog::~TotalBansDialog()
{
    m_storeDialogSize = size();
}

void TotalBansDialog::updateBlockedTodayLabel()
{
    m_blockedTodayLabel->setText(tr("Blocked today by IP filter: %1")
            .arg(QLocale().toString(BanCounter::blockedByIPFilterToday())));
}

void TotalBansDialog::populate()
{
    // Count the manually banned IP addresses per country
    const QStringList bannedIPs = BitTorrent::Session::instance()->bannedIPs();
    const QStringList countries = countriesOf(bannedIPs);
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
    m_list->clear();
    for (const CountryBans &row : asConst(rows))
    {
        auto *item = new QTreeWidgetItem(m_list);
        item->setIcon(0, UIThemeManager::instance()->getFlagIcon(row.code));
        item->setText(0, row.name);
        item->setData(0, COUNTRY_CODE_ROLE, row.code);
        item->setText(1, locale.toString(row.bans));
        item->setTextAlignment(1, (Qt::AlignRight | Qt::AlignVCenter));
    }

    m_totalLabel->setText(tr("Total: %1 bans in %2 countries")
            .arg(locale.toString(bannedIPs.size()), locale.toString(bansPerCountry.size() - (bansPerCountry.contains({}) ? 1 : 0))));
    m_unbanButton->setEnabled(false);
}

void TotalBansDialog::unbanSelectedCountries()
{
    QSet<QString> countryCodes;
    QStringList countryNames;
    const QList<QTreeWidgetItem *> selectedItems = m_list->selectedItems();
    for (const QTreeWidgetItem *item : selectedItems)
    {
        countryCodes.insert(item->data(0, COUNTRY_CODE_ROLE).toString());
        countryNames.append(item->text(0));
    }
    if (countryCodes.isEmpty())
        return;

    // Keep every banned IP address that is not from one of the selected countries
    auto *session = BitTorrent::Session::instance();
    const QStringList bannedIPs = session->bannedIPs();
    const QStringList countries = countriesOf(bannedIPs);
    QStringList keptIPs;
    keptIPs.reserve(bannedIPs.size());
    for (qsizetype i = 0; i < bannedIPs.size(); ++i)
    {
        if (!countryCodes.contains(countries[i]))
            keptIPs.append(bannedIPs[i]);
    }

    const qsizetype unbannedCount = bannedIPs.size() - keptIPs.size();
    if (unbannedCount > 0)
    {
        session->setBannedIPs(keptIPs);
        LogMsg(tr("Unbanned %1 IP address(es) from %2").arg(QString::number(unbannedCount), countryNames.join(u", ")));
    }

    populate();
}
