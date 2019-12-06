#include "file-link-operation.h"
#include "file-operation-manager.h"

#include "gerror-wrapper.h"

#include <QUrl>

using namespace Peony;

FileLinkOperation::FileLinkOperation(QString srcUri, QString destDirUri, QObject *parent) : FileOperation (parent)
{
    m_src_uri = srcUri;
    QUrl url = srcUri;
    m_dest_uri = destDirUri + "/" + url.fileName();
    QStringList fake_uris;
    fake_uris<<srcUri;
    m_info = std::make_shared<FileOperationInfo>(fake_uris, destDirUri, FileOperationInfo::Link);
}

FileLinkOperation::~FileLinkOperation()
{

}

void FileLinkOperation::run()
{
    operationStarted();
    auto destFile = wrapGFile(g_file_new_for_uri(m_dest_uri.toUtf8().constData()));
    GError *err = nullptr;
retry:
    QUrl url = m_src_uri;
    g_file_make_symbolic_link(destFile.get()->get(),
                              url.path().toUtf8().constData(),
                              nullptr,
                              &err);
    if (err) {
        //forbid response actions except retry and cancel.
        auto responseType = errored(m_src_uri, m_dest_uri, GErrorWrapper::wrapFrom(err), true);
        if (responseType.value<FileOperation::ResponseType>() == FileOperation::Retry) {
            goto retry;
        }
    }
    operationFinished();
}
