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

#include "share-page.h"
#include "share-properties-page-plugin.h"
#include <QUrl>

#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QButtonGroup>
#include <PeonyFileInfo>

#include <QTextEdit>

#include <QDebug>

SharePage::SharePage(const QString &uri, QWidget *parent) : QWidget(parent)
{
    QUrl url = uri;
    m_share_info = ShareInfo(url.fileName(), false);
    m_share_info.originalPath = url.path();
    auto manager = Peony::SharePropertiesPagePlugin::getInstance();
    auto infos = manager->getCurrentShareInfos();
    for (auto info : infos) {
        qDebug()<<"compare"<<m_share_info.name<<info.name<<m_share_info.originalPath<<info.originalPath;
        if (m_share_info.name == info.name && info.originalPath == info.originalPath) {
            m_share_info.comment = info.comment;
            m_share_info.readOnly = info.readOnly;
            m_share_info.allowGuest = info.allowGuest;
            m_share_info.originalPath = info.originalPath;
            m_share_info.isShared = true;
            break;
        }
    }

    auto layout = new QFormLayout(this);
    auto info = Peony::FileInfo::fromUri(uri);
    QPushButton *bt = new QPushButton(this);
    bt->setIcon(QIcon::fromTheme(info->iconName()));
    bt->setIconSize(QSize(48, 48));
    QLabel *uriLabel = new QLabel(info->uri(), this);
    uriLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addRow(bt, uriLabel);

    QFrame *s = new QFrame(this);
    s->setFrameShape(QFrame::HLine);
    layout->addRow(s, s);

    bool shared = m_share_info.isShared;

    QCheckBox *shareCheckBox = new QCheckBox(tr("Share folder"), this);
    shareCheckBox->setChecked(m_share_info.isShared);
    layout->addRow(tr("Share:"), shareCheckBox);

    s = new QFrame(this);
    s->setFrameShape(QFrame::HLine);
    layout->addWidget(s);

    QCheckBox *shareReadOnlyCheckBox = new QCheckBox(tr("Read Only"), this);
    shareReadOnlyCheckBox->setChecked(m_share_info.readOnly);

    QCheckBox *shareAllowGuestCheckBox = new QCheckBox(tr("Allow Anonymous"));
    shareAllowGuestCheckBox->setChecked(m_share_info.allowGuest);

    layout->addWidget(shareReadOnlyCheckBox);
    layout->addWidget(shareAllowGuestCheckBox);

    s->setVisible(shared);
    shareReadOnlyCheckBox->setVisible(shared);
    shareAllowGuestCheckBox->setVisible(shared);

    QTextEdit *edit = new QTextEdit(this);
    edit->setText(m_share_info.comment);
    QLabel *comment = new QLabel(tr("Comment:"), this);
    layout->addRow(comment, edit);

    comment->setVisible(shared);
    edit->setVisible(shared);

    connect(edit, &QTextEdit::textChanged, this, [=](){
        m_share_info.comment = edit->toPlainText();
        //FIXME: should i update share info (exec cmd) so frequently?
        NetUsershareHelper::updateShareInfo(m_share_info);
    });

    connect(shareCheckBox, &QCheckBox::clicked, this, [=](bool checked){
        if (checked) {
            m_share_info.name = url.fileName();
            m_share_info.readOnly = shareReadOnlyCheckBox->isChecked();
            m_share_info.allowGuest = shareAllowGuestCheckBox->isChecked();

            NetUsershareHelper::updateShareInfo(m_share_info);

            shareCheckBox->setChecked(m_share_info.isShared);
            shareReadOnlyCheckBox->setVisible(m_share_info.isShared);
            shareAllowGuestCheckBox->setVisible(m_share_info.isShared);
            s->setVisible(m_share_info.isShared);
            comment->setVisible(m_share_info.isShared);
            edit->setVisible(m_share_info.isShared);

            if (m_share_info.isShared) {
                /*!
                  \todo
                  add emblem info to peony-qt's tags(emblems) & cloumns extensions frameworks.
                  before do it, i have to implement that frameworks first...
                  */
            }
        } else {
            s->setVisible(false);
            shareReadOnlyCheckBox->setVisible(false);
            shareAllowGuestCheckBox->setVisible(false);
            comment->setVisible(false);
            edit->setVisible(false);

            NetUsershareHelper::removeShared(m_share_info.name);
            m_share_info.isShared = false;
        }
    });

    connect(shareReadOnlyCheckBox, &QCheckBox::clicked, this, [=](bool checked){
        m_share_info.readOnly = checked;
        NetUsershareHelper::updateShareInfo(m_share_info);
    });

    connect(shareAllowGuestCheckBox, &QCheckBox::clicked, this, [=](bool checked){
        m_share_info.allowGuest = checked;
        NetUsershareHelper::updateShareInfo(m_share_info);
    });
}
