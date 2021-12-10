/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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

#include "file-lauch-dialog.h"

#include "file-launch-action.h"
#include "file-launch-manager.h"

#include "file-info.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDebug>
#include <QPainter>
#include <QApplication>

using namespace Peony;

FileLauchDialog::FileLauchDialog(const QString &uri, QWidget *parent) : QDialog(parent)
{
    m_layout = new QVBoxLayout(this);
    setLayout(m_layout);

    setWindowTitle(tr("Applications"));
    m_layout->addWidget(new QLabel(tr("Choose an Application to open this file"), this));
    m_view = new QListWidget(this);
    m_view->setIconSize(QSize(48, 48));
    //Set the grid size to be the same as the icon size to solve the overlap problem .link bug#94165
    m_view->setGridSize(QSize(48, 48));
    m_view->setUniformItemSizes(true);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_layout->addWidget(m_view, 1);
    m_check_box = new QCheckBox(tr("Set as Default"), this);
    m_layout->addWidget(m_check_box);
    m_button_box = new QDialogButtonBox(QDialogButtonBox::Ok
                                        | QDialogButtonBox::Cancel,
                                        this);
    m_layout->addWidget(m_button_box);

    //add button translate
    m_button_box->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    m_button_box->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    m_uri = uri;
    auto actions = FileLaunchManager::getAllActions(uri);
    for (auto action : actions) {
        //fix show no icon app in list issue, bug#18171
        if (action->icon().isNull())
            continue;
        //FIXME should have a spcific rule to decide which kind of app can show
        //fix show uninstall app in list issue, link to bug#80233
        if (action->icon().name().contains("uninstall"))
            continue;

        action->setParent(this);
        //qDebug() << "lauch actions:" <<action->icon() <<action->iconText() <<action->text();
        auto item = new QListWidgetItem(!action->icon().isNull()? action->icon(): QIcon::fromTheme("application-x-desktop"),
                                        action->text(),
                                        m_view);
        m_view->addItem(item);
        m_hash.insert(item, action);
    }

    connect(this, &QDialog::accepted, [=]() {
        if (m_view->currentItem()) {
            auto action = m_hash.value(m_view->currentItem());
            if (m_check_box->isChecked()) {
                FileLaunchManager::setDefaultLauchAction(m_uri, action);
            }
            action->lauchFileAsync(true);
        } else {
            FileLaunchManager::openAsync(m_uri);
        }
    });

    connect(m_button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    //FIXME: replace BLOCKING api in ui thread.
    auto info = FileInfo::fromUri(uri);
    if (info->isDir() || info->isDesktopFile()) {
        m_check_box->setEnabled(false);
    }
}

void FileLauchDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), qApp->palette().base());
    QWidget::paintEvent(event);
}
