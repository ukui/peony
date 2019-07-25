#include "file-info-manager.h"

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
    std::weak_ptr<FileInfo> weak_info = info;
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
    Q_ASSERT(global_info_list);
    global_info_list->remove(uri);
}

void FileInfoManager::remove(std::shared_ptr<FileInfo> info)
{
    Q_ASSERT(global_info_list);
    global_info_list->remove(info->uri());
}
