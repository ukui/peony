#include "file-item-model.h"
#include "file-item.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-info.h"

#include "file-utils.h"

#include <QDebug>
#include <QMessageBox>
#include <QDate>

using namespace Peony;

FileItemProxyFilterSortModel::FileItemProxyFilterSortModel(QObject *parent) : QSortFilterProxyModel(parent)
{

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
            bool lesser = leftItem->hasChildren();
            if (sortOrder() == Qt::AscendingOrder)
                return lesser;
            return !lesser;
        }

default_sort:
        switch (sortColumn()) {
        case FileItemModel::FileName: {
            QString leftDisplayName = leftItem->m_info->displayName();
            QString rightDisplayName = rightItem->m_info->displayName();
            bool leftStartWithChinese = startWithChinese(leftDisplayName);
            bool rightStartWithChinese = startWithChinese(rightDisplayName);
            //all start with Chinese, use the default compare directly
            if (leftStartWithChinese && rightStartWithChinese)
                break;
            //simplify the logic
            if (leftStartWithChinese || rightStartWithChinese) {
                if (sortOrder() == Qt::AscendingOrder) {
                    return leftStartWithChinese;
                }
                return rightStartWithChinese;
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
             qDebug()<<item->m_info->fileType()<<item->m_info->fileSize()<<item->m_info->modifiedDate()<<item->m_info->size()<<item->m_info->type();
            if (item->m_info->displayName() != nullptr) {
                if (item->m_info->displayName().at(0) == '.')
                    //qDebug()<<sourceRow<<item->m_info->displayName()<<model->rowCount(sourceParent);
                    return false;
            }
        }
        //regExp

        //check the filter conditions
        qDebug()<<"start filter conditions check";
        if (m_show_file_type != ALL_FILE && ! checkFileTypeFilter(item->m_info->type()))
            return false;
        if (m_show_modify_time != ALL_FILE && ! checkFileModifyTimeFilter(item->m_info->modifiedDate()))
            return false;
        if (m_show_file_size != ALL_FILE && ! checkFileSizeFilter(item->m_info->size()))
            return false;
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
    m_show_hidden = showHidden;
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setFilterConditions(int fileType, int modifyTime, int fileSize)
{
    m_show_file_type = fileType;
    m_show_file_size = fileSize;
    m_show_modify_time = modifyTime;
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
