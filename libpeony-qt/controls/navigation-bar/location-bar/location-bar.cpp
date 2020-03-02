/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "location-bar.h"

#include "path-bar-model.h"
#include "file-utils.h"

#include "search-vfs-uri-parser.h"

#include <QUrl>
#include <QMenu>

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionFocusRect>
#include <QLineEdit>

#include <QStandardPaths>

using namespace Peony;

LocationBar::LocationBar(QWidget *parent) : QToolBar(parent)
{
    setToolTip(tr("click the blank area for edit"));

    setStyleSheet("padding-right: 15;"
                  "margin-left: 2");
    m_styled_edit = new QLineEdit;
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setIconSize(QSize(16, 16));
    qDebug()<<sizePolicy();
    //connect(this, &LocationBar::groupChangedRequest, this, &LocationBar::setRootUri);
}

LocationBar::~LocationBar()
{
    m_styled_edit->deleteLater();
}

void LocationBar::setRootUri(const QString &uri)
{
    m_current_uri = uri;

    for (auto action : actions()) {
        removeAction(action);
    }

    if (m_current_uri.startsWith("search://")) {
        QString nameRegexp = SearchVFSUriParser::getSearchUriNameRegexp(m_current_uri);
        QString targetDirectory = SearchVFSUriParser::getSearchUriTargetDirectory(m_current_uri);
        addAction(QIcon::fromTheme("edit-find-symbolic"), tr("Search \"%1\" in \"%2\"").arg(nameRegexp).arg(targetDirectory));
        return;
    }

    QStringList uris;
    QString tmp = uri;
    while (!tmp.isEmpty()) {
        uris.prepend(tmp);
        QUrl url = tmp;
        if (url.path() == QStandardPaths::writableLocation(QStandardPaths::HomeLocation)) {
            break;
        }
        tmp = Peony::FileUtils::getParentUri(tmp);
    }

    for (auto uri : uris) {
        //addButton(uri, uri != uris.last());
        addButton(uri, uris.first() == uri);
    }
}

void LocationBar::addButton(const QString &uri, bool setIcon, bool setMenu)
{
    QAction *action = new QAction(this);
    QUrl url = uri;
    auto parent = FileUtils::getParentUri(uri);
    if (setIcon) {
        QIcon icon = QIcon::fromTheme(Peony::FileUtils::getFileIconName(uri), QIcon::fromTheme("folder"));
        action->setIcon(icon);
    }

    if (!url.fileName().isEmpty()) {
        if (FileUtils::getParentUri(uri).isNull()) {
            setMenu = false;
        }
        action->setText(url.fileName());
    } else {
        if (uri == "file:///") {
            auto text = FileUtils::getFileDisplayName("computer:///root.link");
            if (text.isNull()) {
                text = tr("File System");
            }
            action->setText(text);
        } else {
            action->setText(FileUtils::getFileDisplayName(uri));
        }
    }

    connect(action, &QAction::triggered, [=](){
        //this->setRootUri(uri);
        Q_EMIT this->groupChangedRequest(uri);
    });

    if (setMenu) {
        Peony::PathBarModel m;
        m.setRootUri(uri);
        m.sort(0);

        auto suburis = m.stringList();
        if (!suburis.isEmpty()) {
            QMenu *menu = new QMenu(this);
            QList<QAction *> actions;
            for (auto uri : suburis) {
                QUrl url = uri;
                QString tmp = uri;
                QAction *action = new QAction(url.fileName(), this);
                actions<<action;
                connect(action, &QAction::triggered, [=](){
                    Q_EMIT groupChangedRequest(tmp);
                });
            }
            menu->addActions(actions);

            action->setMenu(menu);
        }
    }

    addAction(action);
}

void LocationBar::mousePressEvent(QMouseEvent *e)
{
    //eat this event.
    //QToolBar::mousePressEvent(e);
    qDebug()<<"black clicked";
    if (e->button() == Qt::LeftButton) {
        Q_EMIT blankClicked();
    }
}

void LocationBar::paintEvent(QPaintEvent *e)
{
    //QToolBar::paintEvent(e);

    QPainter p(this);
    QStyleOptionFocusRect opt;
    opt.initFrom(this);

    //qDebug()<<opt.rect;
    //p.drawRect(opt.rect);
    auto rect = opt.rect;
    rect.setHeight(rect.height() - 1);
    rect.setWidth(rect.width() - 1);
    p.setPen(this->palette().mid().color());
    p.setBrush(m_styled_edit->palette().base());
    p.drawRect(rect);

    style()->drawControl(QStyle::CE_ToolBar, &opt, &p, this);
    //style->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, &p, this);
}
