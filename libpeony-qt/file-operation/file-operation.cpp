#include "file-operation.h"

using namespace Peony;

FileOperation::FileOperation(QObject *parent) : QObject (parent)
{
    m_cancellable_wrapper = wrapGCancellable(g_cancellable_new());
    setAutoDelete(true);
}

FileOperation::~FileOperation()
{

}

void FileOperation::cancel()
{
    g_cancellable_cancel(m_cancellable_wrapper.get()->get());
    m_is_cancelled = true;
}
