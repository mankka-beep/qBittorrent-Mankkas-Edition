/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2020  Prince Gupta <jagannatharjun11@gmail.com>
 * Copyright (C) 2019  sledgehammer999 <hammered999@gmail.com>
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

#include "loglistview.h"

#include <QtSystemDetection>

#include <algorithm>
#include <optional>

#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QFontMetrics>
#include <QHelpEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QToolTip>

#include "base/global.h"
#include "base/net/geoipmanager.h"
#include "gui/uithememanager.h"
#include "logmodel.h"

#ifdef Q_OS_WIN
#include "base/preferences.h"
#endif

namespace
{
    const QString SEPARATOR = u" - "_s;
    // Space around the country flag shown in the peer log
    const int FLAG_MARGIN = 2;

    int horizontalAdvance(const QFontMetrics &fontMetrics, const QString &text)
    {
        return fontMetrics.horizontalAdvance(text);
    }

    QString logText(const QModelIndex &index)
    {
        return index.data(BaseLogModel::TimeRole).toString()
            + SEPARATOR
            + index.data(BaseLogModel::MessageRole).toString();
    }

    class LogItemDelegate final : public QStyledItemDelegate
    {
    public:
        explicit LogItemDelegate(QObject *parent = nullptr)
            : QStyledItemDelegate(parent)
#ifdef Q_OS_WIN
            , m_useCustomUITheme(Preferences::instance()->useCustomUITheme())
#endif
        {
        }

    private:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            painter->save();
            QStyledItemDelegate::paint(painter, option, index); // paints background, focus rect and selection rect

            const QStyle *style = option.widget ? option.widget->style() : QApplication::style();
            const QRect textRect = option.rect.adjusted(1, 0, 0, 0); // shift 1 to avoid text being too close to focus rect
            const bool isEnabled = option.state.testFlag(QStyle::State_Enabled);

#ifdef Q_OS_WIN
            // Windows default theme do not use highlighted text color
            const QPalette::ColorRole textRole = m_useCustomUITheme
                ? (option.state.testFlag(QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::WindowText)
                : QPalette::WindowText;
#else
            const QPalette::ColorRole textRole = option.state.testFlag(QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::WindowText;
#endif

            painter->setFont(itemFont(option));

            QPalette palette = option.palette;

            const QString time = index.data(BaseLogModel::TimeRole).toString();
            palette.setColor(QPalette::Active, QPalette::WindowText, index.data(BaseLogModel::TimeForegroundRole).value<QColor>());
            style->drawItemText(painter, textRect, option.displayAlignment, palette, isEnabled, time, textRole);

            const QFontMetrics fontMetrics = painter->fontMetrics(); // option.fontMetrics adds extra padding to QFontMetrics::width
            const int separatorCoordinateX = horizontalAdvance(fontMetrics, time);
            style->drawItemText(painter, textRect.adjusted(separatorCoordinateX, 0, 0, 0), option.displayAlignment, option.palette
                                , isEnabled, SEPARATOR, textRole);

            const int messageCoordinateX = separatorCoordinateX + horizontalAdvance(fontMetrics, SEPARATOR);
            const QString message = index.data(BaseLogModel::MessageRole).toString();
            palette.setColor(QPalette::Active, QPalette::WindowText, index.data(BaseLogModel::MessageForegroundRole).value<QColor>());

            // Blocked/banned peers: the country flag of the IP address is shown right after the address
            if (const std::optional<FlagLayout> flag = flagLayout(option, index, fontMetrics); !flag)
            {
                style->drawItemText(painter, textRect.adjusted(messageCoordinateX, 0, 0, 0), option.displayAlignment, palette
                                    , isEnabled, message, textRole);
            }
            else
            {
                style->drawItemText(painter, textRect.adjusted(messageCoordinateX, 0, 0, 0), option.displayAlignment, palette
                                    , isEnabled, message.left(flag->textPosition), textRole);

                UIThemeManager::instance()->getFlagIcon(flag->countryCode).paint(painter, flag->rect);

                const int afterFlagCoordinateX = (flag->rect.right() + 1 + FLAG_MARGIN) - textRect.left();
                style->drawItemText(painter, textRect.adjusted(afterFlagCoordinateX, 0, 0, 0), option.displayAlignment, palette
                                    , isEnabled, message.mid(flag->textPosition), textRole);
            }

            painter->restore();
        }

        // Hovering over a flag shows the name of its country
        bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override
        {
            if (event->type() != QEvent::ToolTip)
                return QStyledItemDelegate::helpEvent(event, view, option, index);

            const QFontMetrics fontMetrics {itemFont(option), view->viewport()};
            if (const std::optional<FlagLayout> flag = flagLayout(option, index, fontMetrics); flag && flag->rect.contains(event->pos()))
            {
                QToolTip::showText(event->globalPos(), Net::GeoIPManager::CountryName(flag->countryCode), view->viewport(), flag->rect);
                return true;
            }

            QToolTip::hideText();
            return true;
        }

        struct FlagLayout
        {
            QString countryCode;
            qsizetype textPosition = 0;  // the flag is drawn before this character of the message
            QRect rect;
        };

        // Where the country flag of a blocked/banned peer goes, if the row has one
        static std::optional<FlagLayout> flagLayout(const QStyleOptionViewItem &option, const QModelIndex &index, const QFontMetrics &fontMetrics)
        {
            const QString ip = index.data(BaseLogModel::IPRole).toString();
            const QString countryCode = index.data(BaseLogModel::CountryCodeRole).toString();
            if (ip.isEmpty() || countryCode.isEmpty())
                return std::nullopt;

            const QString message = index.data(BaseLogModel::MessageRole).toString();
            const qsizetype ipPosition = message.indexOf(ip);
            if (ipPosition < 0)
                return std::nullopt;

            // An address in quotes ("1.2.3.4") gets its flag after the closing quote
            qsizetype textPosition = ipPosition + ip.size();
            if ((textPosition < message.size()) && (message[textPosition] == u'"'))
                ++textPosition;

            const QRect textRect = option.rect.adjusted(1, 0, 0, 0);
            const int messageX = textRect.left()
                    + horizontalAdvance(fontMetrics, index.data(BaseLogModel::TimeRole).toString())
                    + horizontalAdvance(fontMetrics, SEPARATOR);
            const int textEndX = messageX + horizontalAdvance(fontMetrics, message.left(textPosition));
            return FlagLayout {countryCode, textPosition, flagRectAt(textEndX, option.rect)};
        }

        // for unknown reasons (fixme) painter won't accept some font properties
        // until they are set explicitly, and we have to manually set some font properties
        static QFont itemFont(const QStyleOptionViewItem &option)
        {
            QFont font = option.font;
            font.setFamily(option.font.family());
            if (option.font.pointSizeF() > 0) // for better scaling we use floating point version
                font.setPointSizeF(option.font.pointSizeF());
            return font;
        }

        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            const QSize minimumFontPadding(4, 4);
            QSize fontSize = option.fontMetrics.size(0, logText(index)) + minimumFontPadding;
            if (!index.data(BaseLogModel::CountryCodeRole).toString().isEmpty())
                fontSize.rwidth() += flagRectAt(0, {0, 0, 0, fontSize.height()}).width() + (2 * FLAG_MARGIN);
            const QSize defaultSize = QStyledItemDelegate::sizeHint(option, index);
            const QSize margins = (defaultSize - fontSize).expandedTo({0, 0});
            return fontSize + margins;
        }

        // A flag that fills the height of the row (minus a small margin), with the 4:3 shape of the flag images
        static QRect flagRectAt(const int textEndX, const QRect &rowRect)
        {
            const int height = std::max((rowRect.height() - (2 * FLAG_MARGIN)), 1);
            const int width = ((height * 4) + 2) / 3;
            return {(textEndX + FLAG_MARGIN), (rowRect.top() + ((rowRect.height() - height) / 2)), width, height};
        }

#ifdef Q_OS_WIN
        const bool m_useCustomUITheme = false;
#endif
    };
}

LogListView::LogListView(QWidget *parent)
    : QListView(parent)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setItemDelegate(new LogItemDelegate(this));

#ifdef Q_OS_MACOS
    setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
}

void LogListView::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy))
        copySelection();
    else
        QListView::keyPressEvent(event);
}

void LogListView::copySelection() const
{
    QStringList list;
    const QModelIndexList selectedIndexes = selectionModel()->selectedRows();
    for (const QModelIndex &index : selectedIndexes)
        list.append(logText(index));
    QApplication::clipboard()->setText(list.join(u'\n'));
}
