#include "clipboard-utils.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>

#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-copy-operation.h"

using namespace Peony;

static ClipboardUtils *global_instance = nullptr;

/*!
 * \brief m_clipboard_parent_uri
 * \note
 * Once we close the current application, the qt will take over the clipboard data.
 * If we open the application, the program will query the clipboard cross the process.
 * This will cost much more time than the first time we open the application.
 * This variable is used to make sure that we always get the clipboard data from current
 * progress when we get the files from clipboard utils.
 */
static QString m_clipboard_parent_uri = nullptr;

ClipboardUtils *ClipboardUtils::getInstance()
{
    if (!global_instance) {
        global_instance = new ClipboardUtils;
    }
    return global_instance;
}

ClipboardUtils::ClipboardUtils(QObject *parent) : QObject(parent)
{
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &ClipboardUtils::clipboardChanged);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, [=](){
        auto data = QApplication::clipboard()->mimeData();
        if (!data->hasFormat("peony-qt/is-cut")) {
            m_clipboard_parent_uri = nullptr;
        }
    });
}

ClipboardUtils::~ClipboardUtils()
{

}

void ClipboardUtils::release()
{
    delete global_instance;
}

const QString ClipboardUtils::getClipedFilesParentUri()
{
    return m_clipboard_parent_uri;
}

void ClipboardUtils::setClipboardFiles(const QStringList &uris, bool isCut)
{
    if (!global_instance) {
        global_instance = new ClipboardUtils;
    }

    m_clipboard_parent_uri = FileUtils::getParentUri(uris.first());

    auto data = new QMimeData;
    QVariant isCutData = QVariant(isCut);
    data->setData("peony-qt/is-cut", isCutData.toByteArray());
    QList<QUrl> urls;
    for (auto uri : uris) {
        urls<<uri;
    }
    data->setUrls(urls);
    QApplication::clipboard()->setMimeData(data);
}

bool ClipboardUtils::isClipboardHasFiles()
{
    return QApplication::clipboard()->mimeData()->hasUrls();
}

bool ClipboardUtils::isClipboardFilesBeCut()
{
    if (isClipboardHasFiles()) {
        auto data = QApplication::clipboard()->mimeData();
        if (data->hasFormat("peony-qt/is-cut")) {
            QVariant var(data->data("peony-qt/is-cut"));
            return var.toBool();
        }
    }
    return false;
}

QStringList ClipboardUtils::getClipboardFilesUris()
{
    QStringList l;

    if (m_clipboard_parent_uri.isNull())
        return l;

    if (!isClipboardHasFiles()) {
        return l;
    }
    auto urls = QApplication::clipboard()->mimeData()->urls();
    for (auto url : urls) {
        l<<url.toString();
    }
    return l;
}

void ClipboardUtils::pasteClipboardFiles(const QString &targetDirUri)
{
    if (!isClipboardHasFiles()) {
        return;
    }
    //auto uris = getClipboardFilesUris();
    auto fileOpMgr = FileOperationManager::getInstance();
    if (isClipboardFilesBeCut()) {
        qDebug()<<getClipboardFilesUris();
        auto moveOp = new FileMoveOperation(getClipboardFilesUris(), targetDirUri);
        fileOpMgr->startOperation(moveOp, true);
        QApplication::clipboard()->clear();
    } else {
        auto copyOp = new FileCopyOperation(getClipboardFilesUris(), targetDirUri);
        fileOpMgr->startOperation(copyOp, true);
    }
}

void ClipboardUtils::clearClipboard()
{
    QApplication::clipboard()->clear();
}
