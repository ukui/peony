#include "file-count-operation.h"

#include "file-node-reporter.h"
#include "file-node.h"

using namespace Peony;

FileCountOperation::FileCountOperation(const QStringList &uris, QObject *parent)
    : FileOperation (parent)
{
    m_reporter = new FileNodeReporter(this);
    connect(m_reporter, &FileNodeReporter::nodeFound, this, &FileOperation::operationPreparedOne);
    m_uris = uris;
}

FileCountOperation::~FileCountOperation()
{

}

void FileCountOperation::cancel()
{
    FileOperation::cancel();
    m_reporter->cancel();
}

void FileCountOperation::run()
{
    Q_EMIT operationStarted();
    if (m_uris.isEmpty())
        Q_EMIT operationFinished();

    for (auto uri : m_uris) {
        auto node = new FileNode(uri, nullptr, m_reporter);
        node->findChildrenRecursively();
        delete node;
    }
    Q_EMIT operationPrepared();
    Q_EMIT operationFinished();
}
