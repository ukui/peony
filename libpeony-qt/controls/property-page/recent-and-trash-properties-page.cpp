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
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QUrl>

using namespace Peony;

RecentAndTrashPropertiesPage::RecentAndTrashPropertiesPage(const QStringList &uris, QWidget *parent) : PropertiesWindowTabIface(parent)
{
    m_uri = uris.first();
    bool startWithTrash = m_uri.startsWith("trash:///");

    //FIXME: replace BLOCKING api in ui thread.
    auto info = FileInfo::fromUri(m_uri);
    if (info->displayName().isEmpty()) {
        FileInfoJob job(info);
        job.querySync();
    }

    m_layout = new QFormLayout(this);
    m_layout->setRowWrapPolicy(QFormLayout::WrapLongRows);
    m_layout->setFormAlignment(Qt::AlignLeft|Qt::AlignHCenter);
    m_layout->setLabelAlignment(Qt::AlignRight|Qt::AlignHCenter);

    auto icon = new QPushButton(QIcon::fromTheme(info->iconName()), nullptr, this);
    icon->setIconSize(QSize(48, 48));
    icon->setProperty("isIcon", true);
    auto name = new QLineEdit(this);
    name->setReadOnly(true);
    name->setText(info->displayName());
    m_layout->addRow(icon, name);
    m_layout->setAlignment(name, Qt::AlignLeft|Qt::AlignCenter);

    addSeparator();

    if (startWithTrash) {
        if (m_uri == "trash:///") {

        } else {
            GFile *file = g_file_new_for_uri(m_uri.toUtf8().constData());
            GFileInfo *info = g_file_query_info(file,
                                                G_FILE_ATTRIBUTE_TRASH_ORIG_PATH,
                                                G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                nullptr,
                                                nullptr);
            auto origin_path = g_file_info_get_attribute_byte_string(info, G_FILE_ATTRIBUTE_TRASH_ORIG_PATH);

            auto label = new QLabel(QString(origin_path), this);
            label->setWordWrap(true);
            m_layout->addRow(tr("Origin Path: "), label);

            g_object_unref(info);
            g_object_unref(file);
        }
    } else {
        if (m_uri == "recent:///") {

        } else {
            //FIXME: replace BLOCKING api in ui thread.
            auto targetUri = FileUtils::getTargetUri(m_uri);
            auto label = new QLabel(QUrl(targetUri).toDisplayString(), this);
            label->setWordWrap(true);
            m_layout->addRow(tr("Size: "), new QLabel(info->fileSize(), this));
            m_layout->addRow(tr("Original Location: "), label);
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

}
