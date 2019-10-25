#include "file-info-manager.h"
#include <QDebug>

using namespace Peony;

static FileInfoManager* global_file_info_manager = nullptr;
static QHash<QString, std::shared_ptr<FileInfo>> *global_info_list = nullptr;

FileInfoManager::FileInfoManager()
{
    global_info_list = new QHash<QString, std::shared_ptr<FileInfo>>();
}

FileInfoManager::~FileInfoManager()
{
    delete global_info_list;
}

FileInfoManager *FileInfoManager::getInstance()
{
    if (!global_file_info_manager)
        global_file_info_manager = new FileInfoManager;
    return global_file_info_manager;
}

std::shared_ptr<FileInfo> FileInfoManager::findFileInfoByUri(QString uri)
{
    Q_ASSERT(global_info_list);
    return global_info_list->value(uri);//.lock();
}

void FileInfoManager::insertFileInfo(std::shared_ptr<FileInfo> info)
{
    Q_ASSERT(global_info_list);
    if (global_info_list->value(info->uri())) {
        qDebug()<<"has info yet";
        return;
    }
    global_info_list->insert(info->uri(), info);
}

void FileInfoManager::removeFileInfobyUri(QString uri)
{
    Q_ASSERT(global_info_list);
    global_info_list->remove(uri);
}

void FileInfoManager::clear()
{
    Q_ASSERT(global_info_list);
    global_info_list->clear();
}

void FileInfoManager::remove(QString uri)
{
    qDebug()<<"remove"<<uri;
    Q_ASSERT(global_info_list);
    global_info_list->remove(uri);
}

void FileInfoManager::remove(std::shared_ptr<FileInfo> info)
{
    Q_ASSERT(global_info_list);
    global_info_list->remove(info->uri());
}

void FileInfoManager::showState()
{
    qDebug()<<global_info_list->keys().count()<<global_info_list->values().count();
}
