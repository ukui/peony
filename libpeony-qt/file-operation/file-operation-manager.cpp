#include "file-operation-manager.h"

using namespace Peony;

static FileOperationManager *global_instance = nullptr;

FileOperationManager::FileOperationManager(QObject *parent) : QObject(parent)
{

}

FileOperationManager::~FileOperationManager()
{

}

FileOperationManager *FileOperationManager::getInstance()
{
    if (global_instance == nullptr) {
        global_instance = new FileOperationManager;
    }
    return global_instance;
}

void FileOperationManager::close()
{
    disconnect();
    deleteLater();
    global_instance = nullptr;
    Q_EMIT closed();
}
