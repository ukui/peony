/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "progress-item-delegate.h"
#include "tablet-app-manager.h"
#include <QDebug>
#include <QColor>
#include "../../tablet/src/Style/style.h"

using namespace Peony;

FullItemDelegate::FullItemDelegate(QObject *parent, int module):
    QStyledItemDelegate(parent)
{
    this->module=module;
}

FullItemDelegate::~FullItemDelegate()
{

}

void FullItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid())
    {
        painter->save();
        QStyleOptionViewItem viewOption(option);//用来在视图中画一个item
        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width());
        rect.setHeight(option.rect.height());


        painter->setOpacity(1);
        TABLETAPP tabletApp = index.data(Qt::DisplayRole).value<TABLETAPP>();
        QString iconstr= tabletApp.appIcon;

        iconstr.remove(".png");
        iconstr.remove(".svg");
        QIcon icon=QIcon::fromTheme(iconstr);
        if (icon.isNull()) {
            icon = ensureLoadIcon(iconstr);
        }
        QString appname= tabletApp.appName;


        QRect iconRect;
        QRect textRect;
        QPixmap pixmap;
        bool bigIcon = index.data(Qt::UserRole).toBool();
        bool appIsDisabled = TabletAppManager::getInstance()->appIsDisabled(tabletApp.execCommand);

        if(bigIcon)
        {
            iconRect=QRect(rect.x(), rect.y()+Style::topSpace-5, rect.width(),Style::BigIconSize);
            textRect=QRect(rect.x(),iconRect.bottom()+5, rect.width(),rect.height()-iconRect.height()-10-5);
            if (appIsDisabled) {
                pixmap = icon.pixmap((Style::BigIconSize ,Style::BigIconSize),QIcon::Disabled,QIcon::Off);
            } else {
                pixmap = icon.pixmap((Style::BigIconSize ,Style::BigIconSize),QIcon::Normal,QIcon::On);
            }
           // pixmap = pixmap.scaled(Style::BigIconSize ,Style::BigIconSize,Qt::KeepAspectRatio);

        }
        else
        {
            iconRect=QRect(rect.x(), rect.y()+Style::topSpace, rect.width(),Style::SmallIconSize);
            textRect=QRect(rect.x(),iconRect.bottom()+10, rect.width() ,rect.height()-iconRect.height()-10-Style::topSpace);
            if (appIsDisabled) {
                pixmap = icon.pixmap((Style::SmallIconSize,Style::SmallIconSize),QIcon::Disabled,QIcon::Off);
            } else {
                pixmap = icon.pixmap((Style::SmallIconSize,Style::SmallIconSize),QIcon::Normal,QIcon::On);
            }
            //pixmap = pixmap.scaled(Style::SmallIconSize ,Style::SmallIconSize,Qt::KeepAspectRatio);

        }
        QApplication::style()->drawItemPixmap(painter,iconRect,Qt::AlignHCenter|Qt::AlignBottom,pixmap);

        painter->restore();
        painter->save();
        painter->setPen(QPen(qApp->palette().color(QPalette::Text)));

        QFontMetrics fm=painter->fontMetrics();
        QString appnameElidedText=fm.elidedText(appname,Qt::ElideRight,textRect.width(),Qt::TextShowMnemonic);
        painter->drawText(textRect,Qt::AlignHCenter,appnameElidedText.toLocal8Bit().data());
        painter->restore();
        painter->save();

        if(option.state & QStyle::State_MouseOver)
        {
            QToolTip::showText(QCursor::pos(),appname);
        }
        else {
            QToolTip::hideText();
        }
        painter->restore();

    }
}

QSize FullItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(Style::itemWidth,Style::itemHeight);
}

QIcon FullItemDelegate::ensureLoadIcon(const QString &iconName) const
{
    QIcon icon;
    if (QFile::exists(QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(iconName).arg("svg")))
        icon = QIcon(QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(iconName).arg("svg"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(iconName).arg("png")))
        icon = QIcon(QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(iconName).arg("png"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/96x96/apps/%1.%2").arg(iconName).arg("png")))
        icon = QIcon(QString("/usr/share/icons/hicolor/96x96/apps/%1.%2").arg(iconName).arg("png"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/96x96/apps/%1.%2").arg(iconName).arg("svg")))
        icon = QIcon(QString("/usr/share/icons/hicolor/96x96/apps/%1.%2").arg(iconName).arg("svg"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(iconName).arg("png")))
        icon = QIcon(QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(iconName).arg("png"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(iconName).arg("svg")))
        icon = QIcon(QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(iconName).arg("svg"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/48x48/apps/%1.%2").arg(iconName).arg("png")))
        icon = QIcon(QString("/usr/share/icons/hicolor/48x48/apps/%1.%2").arg(iconName).arg("png"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/48x48/apps/%1.%2").arg(iconName).arg("svg")))
        icon = QIcon(QString("/usr/share/icons/hicolor/48x48/apps/%1.%2").arg(iconName).arg("svg"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/32x32/apps/%1.%2").arg(iconName).arg("png")))
        icon = QIcon(QString("/usr/share/icons/hicolor/32x32/apps/%1.%2").arg(iconName).arg("png"));
    else if (QFile::exists(QString("/usr/share/icons/hicolor/32x32/apps/%1.%2").arg(iconName).arg("svg")))
        icon = QIcon(QString("/usr/share/icons/hicolor/32x32/apps/%1.%2").arg(iconName).arg("svg"));
    else {
        icon = QIcon::fromTheme(QString("application-x-desktop"));
    }
    return icon;
}

//bool FullItemDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
//{
//    if(event->type()==QEvent::ToolTip)
//    {
//    }
//    else if(event->type()==QEvent::Leave)
//    {
//    }
//    return true;
//}
