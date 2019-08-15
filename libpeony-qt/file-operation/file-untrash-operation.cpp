#include "file-untrash-operation.h"
#include "file-utils.h"

using namespace Peony;

FileUntrashOperation::FileUntrashOperation(QStringList uris, QObject *parent) : FileOperation (parent)
{
    m_uris = uris;
}

void FileUntrashOperation::run()
{
    for (auto uri : m_uris) {
        if (isCancelled())
            break;

        auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
        auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                                    G_FILE_ATTRIBUTE_TRASH_ORIG_PATH,
                                                    G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                    getCancellable().get()->get(),
                                                    nullptr));
        auto originPath = FileUtils::getQStringFromCString(g_file_info_get_attribute_as_string(info.get()->get(),
                                                                              G_FILE_ATTRIBUTE_TRASH_ORIG_PATH));

        auto destFile = wrapGFile(g_file_new_for_path(originPath.toUtf8().constData()));
        auto originUri = FileUtils::getQStringFromCString(g_file_get_uri(destFile.get()->get()));
        m_restore_hash.insert(uri, originUri);

retry:
        GError *err = nullptr;
        g_file_move(file.get()->get(),
                    destFile.get()->get(),
                    m_default_copy_flag,
                    getCancellable().get()->get(),
                    nullptr,
                    nullptr,
                    &err);

        if (err) {
            auto responseData = Q_EMIT errored(uri, originUri, GErrorWrapper::wrapFrom(err), true);
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
}
