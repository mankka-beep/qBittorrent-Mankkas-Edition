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

#include "countrycombobox.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QIcon>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QProxyStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionComboBox>
#include <QStylePainter>

namespace
{
    // Same look as the application style, but the popup is shown as a plain list
    // with a scroll bar (instead of a menu that scrolls only with arrows at its top and bottom)
    class ListPopupComboBoxStyle final : public QProxyStyle
    {
    public:
        using QProxyStyle::QProxyStyle;

        int styleHint(const StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr
                , QStyleHintReturn *returnData = nullptr) const override
        {
            if (hint == QStyle::SH_ComboBox_Popup)
                return 0;
            return QProxyStyle::styleHint(hint, option, widget, returnData);
        }
    };
}

CountryComboBox::CountryComboBox(QWidget *parent)
    : QComboBox(parent)
{
    auto *listPopupStyle = new ListPopupComboBoxStyle(QApplication::style()->name());
    listPopupStyle->setParent(this);
    setStyle(listPopupStyle);

    // Standard item painting, which draws the check box left of the flag
    setItemDelegate(new QStyledItemDelegate(this));
    setMaxVisibleItems(20);
    view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // Clicking an item checks or unchecks it instead of selecting it and closing the popup
    view()->viewport()->installEventFilter(this);
    view()->installEventFilter(this);

    updateToolTip();
}

void CountryComboBox::addCountry(const QIcon &flag, const QString &name, const QString &countryCode)
{
    addItem(flag, name, countryCode);
    setItemData((count() - 1), Qt::Unchecked, Qt::CheckStateRole);
}

QStringList CountryComboBox::checkedCountryCodes() const
{
    QStringList codes;
    for (const int row : checkedRows())
        codes.append(itemData(row).toString());
    return codes;
}

void CountryComboBox::setCheckedCountryCodes(const QStringList &countryCodes)
{
    for (int row = 0; row < count(); ++row)
    {
        const bool checked = countryCodes.contains(itemData(row).toString());
        setItemData(row, (checked ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
    }

    updateToolTip();
    update();
}

QStringList CountryComboBox::checkedCountryNames() const
{
    QStringList names;
    for (const int row : checkedRows())
        names.append(itemText(row));
    return names;
}

bool CountryComboBox::eventFilter(QObject *watched, QEvent *event)
{
    if ((watched == view()->viewport()) && (event->type() == QEvent::MouseButtonRelease))
    {
        const auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (const QModelIndex index = view()->indexAt(mouseEvent->position().toPoint()); index.isValid())
            toggleCheckState(index.row());
        return true;  // keep the popup open
    }

    if ((watched == view()) && (event->type() == QEvent::KeyPress)
        && (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Space))
    {
        if (const QModelIndex index = view()->currentIndex(); index.isValid())
            toggleCheckState(index.row());
        return true;
    }

    return QComboBox::eventFilter(watched, event);
}

void CountryComboBox::paintEvent([[maybe_unused]] QPaintEvent *event)
{
    QStyleOptionComboBox option;
    initStyleOption(&option);

    // Show what is checked instead of the current item
    const QList<int> rows = checkedRows();
    if (rows.isEmpty())
    {
        option.currentText = tr("Select countries");
        option.currentIcon = {};
    }
    else if (rows.size() == 1)
    {
        option.currentText = itemText(rows.first());
        option.currentIcon = itemIcon(rows.first());
    }
    else
    {
        option.currentText = tr("%1 countries").arg(rows.size());
        option.currentIcon = {};
    }

    QStylePainter painter {this};
    painter.drawComplexControl(QStyle::CC_ComboBox, option);
    painter.drawControl(QStyle::CE_ComboBoxLabel, option);
}

void CountryComboBox::toggleCheckState(const int row)
{
    const bool checked = (itemData(row, Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked);
    setItemData(row, (checked ? Qt::Unchecked : Qt::Checked), Qt::CheckStateRole);

    updateToolTip();
    update();
    emit checkedCountriesChanged();
}

QList<int> CountryComboBox::checkedRows() const
{
    QList<int> rows;
    for (int row = 0; row < count(); ++row)
    {
        if (itemData(row, Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked)
            rows.append(row);
    }
    return rows;
}

void CountryComboBox::updateToolTip()
{
    const QStringList names = checkedCountryNames();
    setToolTip(names.isEmpty()
            ? tr("Check the countries whose peers the Ban button bans")
            : tr("Checked: %1").arg(names.join(u", ")));
}
