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
    auto settings = GlobalSettings::getInstance();
    m_show_hidden = settings->isExist("show-hidden")? settings->getValue("show-hidden").toBool(): false;
    m_use_default_name_sort_order = settings->isExist("chinese-first")? !settings->getValue("chinese-first").toBool(): false;
    m_folder_first = settings->isExist("folder-first")? settings->getValue("folder-first").toBool(): true;
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
            if (!m_use_default_name_sort_order) {
                QString leftDisplayName = leftItem->m_info->displayName();
                QString rightDisplayName = rightItem->m_info->displayName();
                bool leftStartWithChinese = startWithChinese(leftDisplayName);
                bool rightStartWithChinese = startWithChinese(rightDisplayName);
                //all start with Chinese, use the default compare directly
                if (leftStartWithChinese && rightStartWithChinese)
                    return comparer.compare(leftDisplayName, rightDisplayName) < 0;
                //simplify the logic
                if (leftStartWithChinese || rightStartWithChinese) {
                    if (sortOrder() == Qt::AscendingOrder) {
                        return leftStartWithChinese;
                    }
                    return rightStartWithChinese;
                }
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
        //qDebug()<<"start filter conditions check";
        if (m_show_file_type != ALL_FILE && ! checkFileTypeFilter(item->m_info->type()))
            return false;
        if (m_show_modify_time != ALL_FILE && ! checkFileModifyTimeFilter(item->m_info->modifiedDate()))
            return false;
        if (m_show_file_size != ALL_FILE && ! checkFileSizeFilter(item->m_info->size()))
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

        //check mutiple label filter conditions, file has any one of these label is accepted
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

bool FileItemProxyFilterSortModel::checkFileTypeFilter(QString type) const
{
    //qDebug()<<"m_show_file_type: "<<m_show_file_type<<" "<<item->info()->type();
    switch (m_show_file_type)
    {
        case FILE_FOLDER:
        {
            if (type != Folder_Type)
                return false;
            break;
        }
        case PICTURE:
        {
            if (! type.contains(Image_Type))
                return false;
            break;
        }
        case VIDEO:
        {
            if (! type.contains(Video_Type))
                return false;
            break;
        }
        case TXT_FILE:
        {
            if (! type.contains(Text_Type))
                return false;
            break;
        }
        case AUDIO:
        {
            if (! type.contains(Audio_Type))
                return false;
            break;
        }
        case OTHERS:
        {
            //exclude classfied types, show the rest other types
            if (type == Folder_Type || type.contains(Image_Type) || type.contains(Video_Type)
                    || type.contains(Text_Type) || type.contains(Audio_Type))
                return false;
            break;
        }
        default:
            break;
    }
    return true;
}

bool FileItemProxyFilterSortModel::checkFileModifyTimeFilter(QString modifiedDate) const
{
    //qDebug()<<"checkFileModifyTimeFilter";
    //invalide date
    if (modifiedDate.size() < 8)
        return false;
    QDate date = QDate::currentDate();
    int year = date.year();
    int month = date.month();
    int day = date.day();
    QString md_year, md_month, md_day;
    auto parts = modifiedDate.split('/', QString::KeepEmptyParts);
    md_year = parts[0];
    md_month = parts[1];
    md_day = parts[2].split(' ', QString::KeepEmptyParts)[0];
    if (md_year.toInt() ==0 || md_month.toInt() ==0 || md_day.toInt() ==0)
        qDebug()<<"date format error:" <<md_year<<" "<<md_month<<" "<<md_day;

    switch(m_show_modify_time)
    {
        case TODAY:
        {
            if (day != md_day.toInt())
                return false;
            break;
        }
        case THIS_WEEK:
        {
            //find a future time, return false
            if (year < md_year.toInt() || (year == md_year.toInt() && month < md_month.toInt())
                    || (year == md_year.toInt() && month == md_month.toInt() && day < md_day.toInt()))
            {
                qDebug()<<"Modify time is a future time, please check your system time is correct!";
                return false;
            }
            if (year - md_year.toInt() >1) //more than a year
                return false;
            if (year - md_year.toInt() ==1)
            {
                if (month+12-md_month.toInt() > 1) //more than a month
                    return false;
                if (month+12-md_month.toInt() == 1 && day+31-md_day.toInt() >= date.dayOfWeek())
                    return false;
            }
            if (month - md_month.toInt() > 1) //more than a month
                return false;
            if (month - md_month.toInt() == 1)
            {
                QDate *modify_data = new QDate(md_year.toInt(), md_month.toInt(), md_day.toInt());
                if (day + modify_data->daysInMonth() - md_day.toInt() >= date.dayOfWeek())
                    return false;
            }
            if (day - md_day.toInt() >= date.dayOfWeek()) //same year,same month
                return false;

            break;
        }
        case THIS_MONTH:
        {
            if (month != md_month.toInt())
                return false;
            break;
        }
        case THIS_YEAR:
        {
            if (year != md_year.toInt())
                return false;
            break;
        }
        case YEAR_AGO:
        {
            if(year == md_year.toInt())
                return false;
            break;
        }
        default:
            break;
    }

    return true;
}

bool FileItemProxyFilterSortModel::checkFileSizeFilter(quint64 size) const
{
    //qDebug()<<"checkFileSizeFilter: "<<m_show_file_size<<" "<<size;
    switch (m_show_file_size)
    {
        case TINY: //[0-16K)
        {
            if (size >= 16 * K_BASE)
                return false;
            break;
        }
        case SMALL:  //[16k-1M]
        {
            if(size < 16 * K_BASE || size >K_BASE * K_BASE)
                return false;
            break;
        }
        case MEDIUM: //(1M-100M]
        {
            if(size <= K_BASE * K_BASE || size >100 * K_BASE * K_BASE)
                return false;
            break;
        }
        case BIG:  //(100M-1G]
        {
            if(size <= 100 * K_BASE * K_BASE || size >K_BASE * K_BASE * K_BASE)
                return false;
            break;
        }
        case LARGE: //>1G
        {
            if (size <= K_BASE * K_BASE * K_BASE)
                return false;
            break;
        }
        default:
            break;
    }

    return true;
}


void FileItemProxyFilterSortModel::update()
{
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setShowHidden(bool showHidden)
{
    GlobalSettings::getInstance()->setValue("show-hidden", showHidden);
    m_show_hidden = showHidden;
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setUseDefaultNameSortOrder(bool use)
{
    GlobalSettings::getInstance()->setValue("chinese-first", !use);
    m_use_default_name_sort_order = use;
    beginResetModel();
    sort(sortColumn()>0? sortColumn(): 0, sortOrder()==Qt::DescendingOrder? Qt::DescendingOrder: Qt::AscendingOrder);
    endResetModel();
}

void FileItemProxyFilterSortModel::setFolderFirst(bool folderFirst)
{
    GlobalSettings::getInstance()->setValue("folder-first", folderFirst);
    m_folder_first = folderFirst;
    beginResetModel();
    sort(sortColumn()>0? sortColumn(): 0, sortOrder()==Qt::DescendingOrder? Qt::DescendingOrder: Qt::AscendingOrder);
    endResetModel();
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
