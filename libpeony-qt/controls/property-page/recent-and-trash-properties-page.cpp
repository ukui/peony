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

#include "recent-and-trash-properties-page.h"

#include "file-info.h"
#include "file-info-job.h"
#include "file-utils.h"
#include "global-settings.h"
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QUrl>

#include <QCheckBox>

using namespace Peony;
//460 - 16 - 16 = 428
#define FIXED_ROW_WIDTH 428;

RecentAndTrashPropertiesPage::RecentAndTrashPropertiesPage(const QStringList &uris, QWidget *parent) : PropertiesWindowTabIface(parent)
{
    m_uri = uris.first();

    QFuture<void> future = QtConcurrent::run([=]() {
        m_fileInfo = FileInfo::fromUri(m_uri);
        if (m_fileInfo->displayName().isEmpty()) {
            FileInfoJob job(m_fileInfo);
            job.querySync();
        }
    });

    m_futureWatcher = new QFutureWatcher<void>();
    m_futureWatcher->setFuture(future);

    connect(m_futureWatcher,&QFutureWatcher<void>::finished,this,&RecentAndTrashPropertiesPage::init);
}

void RecentAndTrashPropertiesPage::init()
{
    if (m_futureWatcher) {
        delete m_futureWatcher;
        m_futureWatcher = nullptr;
    }
    m_layout = new QFormLayout(this);
    m_layout->setRowWrapPolicy(QFormLayout::WrapLongRows);
    m_layout->setFormAlignment(Qt::AlignLeft|Qt::AlignHCenter);
    m_layout->setLabelAlignment(Qt::AlignRight|Qt::AlignHCenter);
    m_layout->setContentsMargins(16,16,16,0);
    this->setLayout(m_layout);

    auto icon = new QPushButton(QIcon::fromTheme(m_fileInfo->iconName()), nullptr, this);
    icon->setIconSize(QSize(48, 48));
    icon->setProperty("isIcon", true);

    QVBoxLayout *boxLayout = new QVBoxLayout(this);
    auto name = new QLineEdit(this);
    name->setReadOnly(true);
    name->setText(m_fileInfo->displayName());

    boxLayout->addWidget(name);
    boxLayout->setAlignment(Qt::AlignBottom);

    m_layout->addRow(icon, boxLayout);
    m_layout->setAlignment(Qt::AlignCenter);

    addSeparator();

    bool startWithTrash = m_uri.startsWith("trash:///");

    if (startWithTrash) {
        if (m_uri == "trash:///") {
            auto checkbox = new QCheckBox(tr("Show confirm dialog while trashing: "));
            m_layout->addWidget(checkbox);
            connect(checkbox, &QCheckBox::toggled, this, [=](bool checked){
                this->setProperty("check", checked);
            });
            auto value = GlobalSettings::getInstance()->getValue("showTrashDialog");
            if (value.isValid()) {
                checkbox->setChecked(value.toBool());
            } else {
                checkbox->setChecked(true);
            }
        } else {
            QLabel *label =new QLabel(this);
            QFuture<void> future = QtConcurrent::run([=]() {
                GFile *file = g_file_new_for_uri(m_uri.toUtf8().constData());
                GFileInfo *info = g_file_query_info(file,
                                                    G_FILE_ATTRIBUTE_TRASH_ORIG_PATH,
                                                    G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                    nullptr,
                                                    nullptr);
                auto origin_path = g_file_info_get_attribute_byte_string(info, G_FILE_ATTRIBUTE_TRASH_ORIG_PATH);

                QUrl url(FileUtils::getParentUri("file://" + QString(origin_path)));

                quint64 width = FIXED_ROW_WIDTH - label->fontMetrics().width(tr("Origin Path: "));
                label->setText(label->fontMetrics().elidedText(url.path(), Qt::ElideMiddle,width));
                label->setWordWrap(true);

                g_object_unref(info);
                g_object_unref(file);
            });

            m_futureWatcher = new QFutureWatcher<void>();
            m_futureWatcher->setFuture(future);

            connect(m_futureWatcher,&QFutureWatcher<void>::finished,this,[=]() {
                m_layout->addRow(tr("Origin Path: "), label);

                if (m_futureWatcher)
                    delete m_futureWatcher;
            });
        }
    } else {
        if (m_uri == "recent:///") {

        } else {
            QLabel *sizeLabel =new QLabel(this);
            QLabel *locationLabel =new QLabel(this);
            QFuture<void> future = QtConcurrent::run([=]() {
                auto targetUri = FileUtils::getTargetUri(m_uri);
                quint64 width = FIXED_ROW_WIDTH - locationLabel->fontMetrics().width(tr("Original Location: "));
                locationLabel->setText(locationLabel->fontMetrics().elidedText(QUrl(targetUri).toDisplayString(), Qt::ElideMiddle,width));
                locationLabel->setWordWrap(true);

                sizeLabel->setText(m_fileInfo->fileSize());
            });

            m_futureWatcher = new QFutureWatcher<void>();
            m_futureWatcher->setFuture(future);

            connect(m_futureWatcher,&QFutureWatcher<void>::finished,this,[=]() {
                m_layout->addRow(tr("Size: "), sizeLabel);
                m_layout->addRow(tr("Original Location: "), locationLabel);

                if (m_futureWatcher)
                    delete m_futureWatcher;
            });
        }
    }
}

void RecentAndTrashPropertiesPage::addSeparator()
{
    auto separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    m_layout->addRow(separator);
}

void RecentAndTrashPropertiesPage::saveAllChange()
{
    bool check = this->property("check").toBool();
    GlobalSettings::getInstance()->setValue("showTrashDialog", check);
}
