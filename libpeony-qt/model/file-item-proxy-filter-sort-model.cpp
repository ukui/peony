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
 * Authors: Meihong He <hemeihong@kylinos.cn>
 *
 */

#include "file-item-model.h"
#include "file-item.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-info.h"
#include "file-meta-info.h"
#include "file-label-model.h"

#include "file-utils.h"
#include "file-operation-utils.h"

#include "global-settings.h"

#include <QDebug>
#include <QMessageBox>
#include <QDate>

#include <QLocale>
#include <QCollator>

using namespace Peony;

QLocale locale = QLocale(QLocale::system().name());
QCollator comparer = QCollator(locale);

FileItemProxyFilterSortModel::FileItemProxyFilterSortModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    //enable number sort, like 100 is after 99
    comparer.setNumericMode(true);
    auto settings = GlobalSettings::getInstance();
    m_show_hidden = settings->isExist(SHOW_HIDDEN_PREFERENCE)? settings->getValue(SHOW_HIDDEN_PREFERENCE).toBool(): false;
    m_use_default_name_sort_order = settings->isExist(SORT_CHINESE_FIRST)? settings->getValue(SORT_CHINESE_FIRST).toBool(): false;
    m_folder_first = settings->isExist(SORT_FOLDER_FIRST)? settings->getValue(SORT_FOLDER_FIRST).toBool(): true;
}

void FileItemProxyFilterSortModel::setSourceModel(QAbstractItemModel *model)
{
    if (sourceModel())
        disconnect(sourceModel());
    QSortFilterProxyModel::setSourceModel(model);
    FileItemModel *file_item_model = static_cast<FileItemModel*>(model);
    connect(file_item_model, &FileItemModel::updated, this, &FileItemProxyFilterSortModel::update);
}

FileItem *FileItemProxyFilterSortModel::itemFromIndex(const QModelIndex &proxyIndex)
{
    FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
    QModelIndex index = mapToSource(proxyIndex);
    return model->itemFromIndex(index);
}

QModelIndex FileItemProxyFilterSortModel::getSourceIndex(const QModelIndex &proxyIndex)
{
    return mapToSource(proxyIndex);
}

const QModelIndex FileItemProxyFilterSortModel::indexFromUri(const QString &uri)
{
    FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
    const QModelIndex sourceIndex = model->indexFromUri(uri);
    return mapFromSource(sourceIndex);
}

bool FileItemProxyFilterSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    //comment these improve code to fix disorder issue
    //and chinese first or folder first has not effect issue
    //Fix me with better solutions
//    if (left.data().isNull())
//        return sortOrder() == Qt::AscendingOrder? false: true;

//    if (right.data().isNull())
//        return sortOrder() == Qt::AscendingOrder? true: false;

//    if (left.column() != 0 || right.column() != 0) {
//        return true;
//    }

    //qDebug()<<left<<right;
    if (left.isValid() && right.isValid()) {
        FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
        auto leftItem = model->itemFromIndex(left);
        auto rightItem = model->itemFromIndex(right);
        if (!(leftItem->hasChildren() && rightItem->hasChildren())) {
            //make folder always has a higher order.
            if (!leftItem->hasChildren() && !rightItem->hasChildren()) {
                goto default_sort;
            }
            if (m_folder_first) {
                bool lesser = leftItem->hasChildren();
                if (sortOrder() == Qt::AscendingOrder)
                    return lesser;
                return !lesser;
            }
        }

default_sort:
        switch (sortColumn()) {
        case FileItemModel::FileName: {
            if (FileOperationUtils::leftNameIsDuplicatedFileOfRightName(leftItem->m_info->displayName(), rightItem->m_info->displayName())) {
                return FileOperationUtils::leftNameLesserThanRightName(leftItem->info()->displayName(), rightItem->info()->displayName());
            }
            if (m_use_default_name_sort_order) {
                QString leftDisplayName = leftItem->m_info->displayName();
                QString rightDisplayName = rightItem->m_info->displayName();
                return comparer.compare(leftDisplayName, rightDisplayName) < 0;
            }
            return leftItem->m_info->displayName().toLower() < rightItem->m_info->displayName().toLower();
        }
        case FileItemModel::FileSize: {
            return leftItem->m_info->size() < rightItem->m_info->size();
        }
        case FileItemModel::FileType: {
            return leftItem->m_info->fileType() < rightItem->m_info->fileType();
        }
        case FileItemModel::ModifiedDate: {
            return leftItem->m_info->modifiedTime() < rightItem->m_info->modifiedTime();
        }
        default:
            break;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

bool FileItemProxyFilterSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    //FIXME:
    FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
    //root
    auto childIndex = model->index(sourceRow, 0, sourceParent);
    if (childIndex.isValid()) {
        auto item = static_cast<FileItem*>(childIndex.internalPointer());
        if(!item->shouldShow())
            return false;
        if (!m_show_hidden) {
            //qDebug()<<sourceRow<<item->m_info->displayName()<<model->rowCount(sourceParent);
            //QMessageBox::warning(nullptr, "filter", item->m_info->displayName());
            //qDebug()<<item->m_info->displayName();
            if (item->m_info->displayName() != nullptr) {
                if (item->m_info->displayName().at(0) == '.')
                    //qDebug()<<sourceRow<<item->m_info->displayName()<<model->rowCount(sourceParent);
                    return false;
            }
        }
        //regExp

        //check the file info filter conditions
        //qDebug()<<"start filter conditions check"<<item->m_info->displayName()<<item->m_info->type();
        if (! checkFileTypeFilter(item->m_info->type()))
            return false;
        if (! checkFileModifyTimeFilter(item->m_info->modifiedTime()))
            return false;
        if (! checkFileSizeFilter(item->m_info->size()))
            return false;
        if (! checkFileNameFilter(item->m_info->displayName()))
            return false;

        //check the file label filter conditions
        if (m_label_name != "" || m_label_color != Qt::transparent)
        {
            QString uri = item->m_info->uri();
            if (m_label_name != "")
            {
                auto names = FileLabelModel::getGlobalModel()->getFileLabels(uri);
                if (! names.contains(m_label_name))
                    return false;
            }

            if (m_label_color != Qt::transparent)
            {
                auto colors = FileLabelModel::getGlobalModel()->getFileColors(uri);
                if (! colors.contains(m_label_color))
                    return false;
            }
        }

        //check multiple label filter conditions, file has any one of these label is accepted
        if(m_show_label_names.size() >0 || m_show_label_colors.size() >0)
        {
            bool bfind = false;
            QString uri = item->m_info->uri();
            if (m_show_label_names.size() >0 )
            {
                auto names = FileLabelModel::getGlobalModel()->getFileLabels(uri);
                for(auto temp : m_show_label_names)
                {
                    if(names.contains(temp))
                    {
                        bfind = true;
                        break;
                    }
                }
            }

            if (! bfind && m_show_label_colors.size() >0)
            {
                auto colors = FileLabelModel::getGlobalModel()->getFileColors(uri);
                for(auto temp : m_show_label_colors)
                {
                    if (colors.contains(temp))
                    {
                        bfind = true;
                        break;
                    }
                }
            }

            if (! bfind)
                return false;
        }

        //check the blur name, can use as search color labels
        if (m_blur_name != "")
        {
            QString uri = item->m_info->uri();
            auto names = FileLabelModel::getGlobalModel()->getFileLabels(uri);
            bool find = false;
            for(auto temp : names)
            {
                if ((m_case_sensitive && temp.indexOf(m_blur_name) >= 0) ||
                        (! m_case_sensitive && temp.toLower().indexOf(m_blur_name.toLower()) >= 0))
                {
                    find = true;
                    break;
                }
            }
            if (! find)
                return false;
        }
    }
    return true;
}

bool FileItemProxyFilterSortModel::checkFileNameFilter(const QString &displayName) const
{
    if (m_file_name_list.size() == 0)
        return true;

    for(auto key:m_file_name_list)
    {
        if (displayName.contains(key))
            return true;
    }

    return false;
}

bool FileItemProxyFilterSortModel::checkFileTypeFilter(QString type) const
{
    //qDebug()<<"m_show_file_type: "<<m_show_file_type<<" "<<type;
    //multiple condition, advance search and default search
    if (m_show_file_type == ALL_FILE && m_file_type_list.count() == 0
        || m_file_type_list.contains(ALL_FILE))
        return true;

    //support multiple file type choose
    QList<int> totalTypeList;
    if (m_file_type_list.count() > 0)
        totalTypeList.append(m_file_type_list);
    if (! totalTypeList.contains(m_show_file_type) && m_show_file_type != ALL_FILE)
        totalTypeList.append(m_show_file_type);

    for(int i=0; i<totalTypeList.count(); i++)
    {
        auto cur = totalTypeList[i];
        switch (cur)
        {
        case FILE_FOLDER:
        {
            if (type == Folder_Type)
                return true;
            break;
        }
        case PICTURE:
        {
            if (type.contains(Image_Type))
                return true;
            break;
        }
        case VIDEO:
        {
            if (type.contains(Video_Type))
                return true;
            break;
        }
        case TXT_FILE:
        {
            if (type.contains(Text_Type))
                return true;
            break;
        }
        case WPS_FILE:
        {
            if (type.contains(Wps_Type))
                return true;
            break;
        }
        case AUDIO:
        {
            if (type.contains(Audio_Type))
                return true;
            break;
        }
        case OTHERS:
        {
            //exclude classfied types, show the rest other types
            if (type != Folder_Type && ! type.contains(Image_Type) && ! type.contains(Video_Type)
                    && ! type.contains(Text_Type) && !type.contains(Wps_Type) && ! type.contains(Audio_Type))
                return true;
            break;
        }
        default:
            break;
        }
    }

    return false;
}

bool FileItemProxyFilterSortModel::checkFileModifyTimeFilter(quint64 modifiedTime) const
{
    //qDebug()<<"checkFileModifyTimeFilter";
    //have no date filter
    if (m_modify_time_list.count() == 0 || m_modify_time_list.contains(ALL_FILE))
        return true;

    auto time = QDateTime::currentMSecsSinceEpoch();
    auto dateTime = QDateTime::fromMSecsSinceEpoch(time);
    //qDebug() << "cur QDateTime:" <<dateTime.toString(Qt::SystemLocaleShortDate);
    QDate date = dateTime.date();
    int year = date.year();
    int month = date.month();
    int day = date.day();
    QDate md_date =  QDateTime::fromMSecsSinceEpoch(modifiedTime * 1000).date();
    //qDebug() << "modify time:" <<QDateTime::fromMSecsSinceEpoch(modifiedTime *1000).toString(Qt::SystemLocaleShortDate);
    int md_year= md_date.year();
    int md_month = md_date.month();
    int md_day = md_date.day();

    for(int i=0; i<m_modify_time_list.size(); i++)
    {
        auto cur = m_modify_time_list[i];
        switch(cur)
        {
        case TODAY:
        {
            if (year == md_year && month == md_month && day == md_day)
                return true;
            break;
        }
        case THIS_WEEK:
        {
            QDate md_date(md_year, md_month, md_day);
            int week, md_week,week_year, md_week_year;
            week = date.weekNumber(&week_year);
            md_week = md_date.weekNumber(&md_week_year);
            if (week == md_week && week_year == md_week_year)
                return true;
            break;
        }
        case THIS_MONTH:
        {
            if (year == md_year && month == md_month)
                return true;
            break;
        }
        case THIS_YEAR:
        {
            if (year == md_year)
                return true;
            break;
        }
        case YEAR_AGO:
        {
            if(year > md_year)
                return true;
            break;
        }
        default:
            break;
        }
    }

    return false;
}

bool FileItemProxyFilterSortModel::checkFileSizeFilter(quint64 size) const
{
    //qDebug()<<"checkFileSizeFilter: "<<m_show_file_size<<" "<<size;
    if (m_file_size_list.count() == 0 || m_file_size_list.contains(ALL_FILE))
        return true;

    for(int i=0; i<m_file_size_list.count(); i++)
    {
        auto cur = m_file_size_list[i];
        switch (cur)
        {
        case TINY: //[0-16K)
        {
            if (size < 16 * K_BASE)
                return true;
            break;
        }
        case SMALL:  //[16k-1M]
        {
            if(size >= 16 * K_BASE && size <=K_BASE * K_BASE)
                return true;
            break;
        }
        case MEDIUM: //(1M-100M]
        {
            if(size > K_BASE * K_BASE && size <= 100 * K_BASE * K_BASE)
                return true;
            break;
        }
        case BIG:  //(100M-1G]
        {
            if(size > 100 * K_BASE * K_BASE && size <= K_BASE * K_BASE * K_BASE)
                return true;
            break;
        }
        case LARGE: //>1G
        {
            if (size > K_BASE * K_BASE * K_BASE)
                return true;
            break;
        }
        default:
            break;
        }
    }

    return false;
}


void FileItemProxyFilterSortModel::update()
{
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setShowHidden(bool showHidden)
{
    GlobalSettings::getInstance()->setValue(SHOW_HIDDEN_PREFERENCE, showHidden);
    m_show_hidden = showHidden;
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setUseDefaultNameSortOrder(bool use)
{
    GlobalSettings::getInstance()->setValue(SORT_CHINESE_FIRST, use);
    m_use_default_name_sort_order = use;
    beginResetModel();
    sort(sortColumn()>0? sortColumn(): 0, sortOrder()==Qt::DescendingOrder? Qt::DescendingOrder: Qt::AscendingOrder);
    endResetModel();
}

void FileItemProxyFilterSortModel::setFolderFirst(bool folderFirst)
{
    GlobalSettings::getInstance()->setValue(SORT_FOLDER_FIRST, folderFirst);
    m_folder_first = folderFirst;
    beginResetModel();
    sort(sortColumn()>0? sortColumn(): 0, sortOrder()==Qt::DescendingOrder? Qt::DescendingOrder: Qt::AscendingOrder);
    endResetModel();
}

void FileItemProxyFilterSortModel::addFileNameFilter(QString key, bool updateNow)
{
    m_file_name_list.append(key);
    if (updateNow)
        invalidateFilter();
}

void FileItemProxyFilterSortModel::addFilterCondition(int option, int classify, bool updateNow)
{
    switch (option) {
    case 0:
        if (! m_file_type_list.contains(classify))
            m_file_type_list.append(classify);

        break;
    case 1:
        if (! m_file_size_list.contains(classify))
            m_file_size_list.append(classify);
        break;
    case 2:
        if (! m_modify_time_list.contains(classify))
            m_modify_time_list.append(classify);
        break;
    default:
        break;
    }

    if (updateNow)
        invalidateFilter();
}

void FileItemProxyFilterSortModel::removeFilterCondition(int option, int classify, bool updateNow)
{
    switch (option) {
    case 1:
        if (! m_file_type_list.contains(classify))
            m_file_type_list.removeOne(classify);

        break;
    case 2:
        if (! m_modify_time_list.contains(classify))
            m_modify_time_list.removeOne(classify);
        break;
    case 3:
        if (! m_file_size_list.contains(classify))
            m_file_size_list.removeOne(classify);
        break;
    default:
        break;
    }

    if (updateNow)
        invalidateFilter();
}

void FileItemProxyFilterSortModel::clearConditions()
{
    m_file_name_list.clear();
    m_file_type_list.clear();
    m_file_size_list.clear();
    m_modify_time_list.clear();
}

void FileItemProxyFilterSortModel::setFilterConditions(int fileType, int modifyTime, int fileSize)
{
    m_show_file_type = fileType;
    m_show_file_size = fileSize;
    m_show_modify_time = modifyTime;
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setFilterLabelConditions(QString name, QColor color)
{
    m_label_name = name;
    m_label_color = color;
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setMutipleLabelConditions(QStringList names, QList<QColor> colors)
{
    m_show_label_names.clear();
    m_show_label_colors.clear();

    for(auto name : names)
    {
        m_show_label_names.append(name);
    }
    for(auto color : colors)
    {
        m_show_label_colors.append(color);
    }
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setLabelBlurName(QString blurName, bool caseSensitive)
{
    m_blur_name = blurName;
    m_case_sensitive = caseSensitive;
    invalidateFilter();
}

bool FileItemProxyFilterSortModel::startWithChinese(const QString &displayName) const
{
    //NOTE: a newly created file might could not get display name soon.
    if (displayName.isEmpty()) {
        return false;
    }
    auto firstStrUnicode = displayName.at(0).unicode();
    return (firstStrUnicode <=0x9FA5 && firstStrUnicode >= 0x4E00);
}

QModelIndexList FileItemProxyFilterSortModel::getAllFileIndexes()
{
    //FIXME: how about the tree?
    QModelIndexList l;
    int i = 0;
    while (this->index(i, 0, QModelIndex()).isValid()) {
        auto index = this->index(i, 0, QModelIndex());
        if (m_show_hidden) {
            l<<index;
        } else {
            auto disyplayName = index.data(Qt::DisplayRole).toString();
            if (disyplayName.isEmpty()) {
                auto uri = this->index(i, 0, QModelIndex()).data(FileItemModel::UriRole).toString();
                disyplayName = FileUtils::getFileDisplayName(uri);
            }
            if (!disyplayName.startsWith(".")) {
                l<<index;
            }
        }

        i++;
    }
    return l;
}

QStringList FileItemProxyFilterSortModel::getAllFileUris()
{
    QStringList l;
    auto indexes = getAllFileIndexes();
    for (auto index : indexes) {
        if (index.column() == 0)
            l<<index.data(FileItemModel::UriRole).toString();
    }
    return l;
}
