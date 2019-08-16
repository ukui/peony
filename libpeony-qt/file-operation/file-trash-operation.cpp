#include "file-trash-operation.h"
#include "file-operation-manager.h"

using namespace Peony;

FileTrashOperation::FileTrashOperation(QStringList srcUris, QObject *parent) : FileOperation (parent)
{
    m_src_uris = srcUris;
    m_info = std::make_shared<FileOperationInfo>(srcUris, "trash:///", FileOperationInfo::Trash);
}

void FileTrashOperation::run()
{
    Q_EMIT operationStarted();
    for (auto src : m_src_uris) {
        if (isCancelled())
            break;
        retry:
        auto srcFile = wrapGFile(g_file_new_for_uri(src.toUtf8().constData()));
        GError *err = nullptr;
        g_file_trash(srcFile.get()->get(),
                     getCancellable().get()->get(),
                     &err);
        if (err) {
            auto responseData = Q_EMIT errored(src, tr("trash:///"), GErrorWrapper::wrapFrom(err), true);
            switch (responseData.value<ResponseType>()) {
            case Retry:
                goto retry;
            case Cancel:
                cancel();
                break;
            default:
                break;
            }
        }
    }
    Q_EMIT operationFinished();
}
