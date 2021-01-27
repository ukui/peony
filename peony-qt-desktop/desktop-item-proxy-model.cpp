/*
 * Peony-Qt
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "desktop-item-proxy-model.h"
#include "file-info.h"
#include "file-meta-info.h"

#include "file-operation-utils.h"
#include "global-settings.h"

#include <QDir>
#include <QDebug>

#include <QLocale>
#include <QCollator>
#include <QStandardPaths>
#include <QUrl>

using namespace Peony;

QLocale locale = QLocale(QLocale::system().name());
QCollator comparer = QCollator(locale);

bool startWithChinese(const QString &displayName)
{
    //NOTE: a newly created file might could not get display name soon.
    if (displayName.isEmpty()) {
        return false;
    }
    auto firstStrUnicode = displayName.at(0).unicode();
    return (firstStrUnicode <=0x9FA5 && firstStrUnicode >= 0x4E00);
}

DesktopItemProxyModel::DesktopItemProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    //enable number sort, like 100 is after 99
    comparer.setNumericMode(true);

    setSortCaseSensitivity(Qt::CaseInsensitive);
    setDynamicSortFilter(false);
    auto settings = GlobalSettings::getInstance();
    m_show_hidden = settings->isExist(SHOW_HIDDEN_PREFERENCE)? settings->getValue(SHOW_HIDDEN_PREFERENCE).toBool(): false;
    //qDebug() <<"DesktopItemProxyModel:" <<settings->isExist(SHOW_HIDDEN_PREFERENCE)<<m_show_hidden;

    m_bwListInfo = new BWListInfo();
    m_jsonOp = new PeonyJsonOperation();
    QString jsonPath=QDir::homePath()+"/.config/peony-security-config.json";
    m_jsonOp->setConfigFile(jsonPath);
    m_jsonOp->loadConfigFile(m_bwListInfo);
}

DesktopItemProxyModel::~DesktopItemProxyModel()
{
    delete m_jsonOp;
    delete m_bwListInfo;
}

bool DesktopItemProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel())
        return false;

    auto sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    auto uri = sourceIndex.data(Qt::UserRole).toString();
    auto info = FileInfo::fromUri(uri);
    //qDebug()<<"fiter"<<uri<<info->displayName();
    if (info->displayName().isNull()) {
        //return false;
    }
    if (! m_show_hidden && info->displayName().startsWith(".")) {
        return false;
    }

    //fix desktop show Desktop folder issue, bug#20293
    if (QUrl(uri).path() == QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/Desktop")
        return false;

    if (info->isDesktopFile() && nullptr != info->desktopName()){
        if (m_bwListInfo->isBlackListMode()){
            return !m_bwListInfo->desktopNameExist(info->desktopName());
        } else if (m_bwListInfo->isWriteListMode()){
            return m_bwListInfo->desktopNameExist(info->desktopName());
        }
    }

    return true;
}

bool DesktopItemProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    //qDebug()<<"less than";
    if (m_sort_type == Other) {
        return true;
    }

    auto leftUri = source_left.data(Qt::UserRole).toString();
    auto leftInfo = FileInfo::fromUri(leftUri);
    auto leftMetaInfo = FileMetaInfo::fromUri(leftUri);

    auto rightUri = source_right.data(Qt::UserRole).toString();
    auto rightInfo = FileInfo::fromUri(rightUri);
    auto rightMetaInfo = FileMetaInfo::fromUri(rightUri);

    //computer home and trash first
    if (source_left.row() < 3) {
        if (source_right.row() < source_left.row()) {
            return (sortOrder()==Qt::AscendingOrder)? false: true;
        }
        return (sortOrder()==Qt::AscendingOrder)? true: false;
    }
    if (source_right.row() < 3) {
        if (source_left.row() < source_right.row()) {
            return (sortOrder()==Qt::AscendingOrder)? true: false;
        }
        return (sortOrder()==Qt::AscendingOrder)? false: true;
    }

    //dir first
    if (leftInfo->isDir()) {
        if (rightInfo->isDir()) {
            //do nothing
        } else {
            return (sortOrder()==Qt::AscendingOrder)? true: false;
        }
    } else {
        if (rightInfo->isDir()) {
            return (sortOrder()==Qt::AscendingOrder)? false: true;
        }
    }

    //qDebug()<<"sort in desktop"<<SortType(m_sort_type)<<m_sort_type;
    switch (m_sort_type) {
    case FileName: {
        if (FileOperationUtils::leftNameIsDuplicatedFileOfRightName(leftInfo->displayName(), rightInfo->displayName())) {
            return FileOperationUtils::leftNameLesserThanRightName(leftInfo->displayName(), rightInfo->displayName());
        }
        if (startWithChinese(leftInfo->displayName())) {
            if (!startWithChinese(rightInfo->displayName())) {
                return (sortOrder()==Qt::AscendingOrder)? true: false;
            } else {
                //chinese pinyin sort order is reversed compared with english.
                return !QSortFilterProxyModel::lessThan(source_left, source_right);
            }
        } else {
            if (startWithChinese(rightInfo->displayName())) {
                return (sortOrder()==Qt::AscendingOrder)? false: true;
            }
        }
        return comparer.compare(leftInfo->displayName(), rightInfo->displayName()) < 0;
        //return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
    case ModifiedDate: {
        return leftInfo->modifiedTime() < rightInfo->modifiedTime();
    }
    case FileType: {
        return leftInfo->type() < rightInfo->type();
    }
    case FileSize: {
        return leftInfo->size() < rightInfo->size();
    }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

void DesktopItemProxyModel::setShowHidden(bool showHidden)
{
    GlobalSettings::getInstance()->setValue(SHOW_HIDDEN_PREFERENCE, showHidden);
    m_show_hidden = showHidden;
    invalidateFilter();
}

int DesktopItemProxyModel::updateBlackAndWriteLists()
{
    m_bwListInfo->clearBWlist();
    m_jsonOp->loadConfigFile(m_bwListInfo);
    //重新过滤显示
    invalidateFilter();
    return 0;
}
