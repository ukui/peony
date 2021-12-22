/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "clipboard-utils.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QStandardPaths>

#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-copy-operation.h"

#include <glib.h>
#include <glib/gi18n.h>

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

static bool m_is_desktop_cut = false;
static bool m_is_peony_cut = false;

static QList<QString> m_target_directory_uri;

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
    connect(QApplication::clipboard(), &QClipboard::dataChanged, [=]() {
        auto data = QApplication::clipboard()->mimeData();
        if (!data->hasFormat("peony-qt/is-cut")) {
            m_clipboard_parent_uri = nullptr;
        }
    });
}

ClipboardUtils::~ClipboardUtils()
{
    m_target_directory_uri.clear();
}

void ClipboardUtils::release()
{
    delete global_instance;
}

const QString ClipboardUtils::getClipedFilesParentUri()
{
    return m_clipboard_parent_uri;
}

const QString ClipboardUtils::getLastTargetDirectoryUri()
{
    return m_target_directory_uri.size() > 0 ? m_target_directory_uri.back() : "";
}

void ClipboardUtils::setClipboardFiles(const QStringList &uris, bool isCut)
{
    if (!global_instance) {
        global_instance = new ClipboardUtils;
    }

    if (uris.isEmpty()) {
        clearClipboard();
        return;
    }

    m_clipboard_parent_uri = FileUtils::getParentUri(uris.first());
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (FileUtils::isSamePath(m_clipboard_parent_uri, desktopPath) && isCut){
         m_is_desktop_cut = true;
         m_is_peony_cut = true;
    }
    else if (isCut)
    {
        m_is_desktop_cut = false;
        m_is_peony_cut = true;
    }else{
        m_is_desktop_cut = false;
        m_is_peony_cut = false;
    }


    // we should remain the encoded uri for file operation
    auto data = new QMimeData;
    QVariant isCutData = QVariant(isCut);
    data->setData("peony-qt/is-cut", isCutData.toByteArray());
    QList<QUrl> urls;
    QStringList encodedUris;
    for (auto uri : uris) {
        auto encodeUrl = Peony::FileUtils::urlEncode(uri);
        urls << QString(encodeUrl);
        encodedUris << QString(encodeUrl);
    }
    data->setUrls(urls);
    QString string = encodedUris.join(" ");
    data->setData("peony-qt/encoded-uris", string.toUtf8());
    data->setText(string);
    QApplication::clipboard()->setMimeData(data);
}

bool ClipboardUtils::isClipboardHasFiles()
{
    return QApplication::clipboard()->mimeData()->hasUrls();
}

bool ClipboardUtils::isDesktopFilesBeCut()
{
    return m_is_desktop_cut;
}

bool ClipboardUtils::isPeonyFilesBeCut()
{
    return m_is_peony_cut;
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

    if (!isClipboardHasFiles()) {
        return l;
    }

    auto mimeData = QApplication::clipboard()->mimeData();
    //auto text = mimeData->text();
    auto peonyText = mimeData->data("peony-qt/encoded-uris");

    if (!peonyText.isEmpty()) {
        qDebug() << "peony text:" << peonyText;
        auto byteArrays = peonyText.split(' ');
        for (auto byteArray : byteArrays) {
            l<<byteArray;
        }
    } else {
        auto urls = mimeData->urls();
        for (auto url : urls) {
            g_autofree gchar* uri = g_uri_unescape_string(url.toString().toUtf8().constData(), nullptr);
            l<<QString(uri);
        }
    }

    return l;
}

FileOperation *ClipboardUtils::pasteClipboardFiles(const QString &targetDirUri)
{
    FileOperation *op = nullptr;
    if (!isClipboardHasFiles()) {
        return op;
    }
    //check existed
    auto uris = getClipboardFilesUris();
//    for (auto uri : getClipboardFilesUris()) {
//        //FIXME: replace BLOCKING api in ui thread.
//        if (!FileUtils::isFileExsit(uri)) {
//            uris.removeAll(uri);
//        }
//    }
    if (uris.isEmpty()) {
        return op;
    }

    auto parentPath = FileUtils::getParentUri(uris.first());
    //paste file in old path, return op
    if (FileUtils::isSamePath(parentPath, targetDirUri) && isClipboardFilesBeCut())
    {
        clearClipboard();
        return op;
    }

    auto fileOpMgr = FileOperationManager::getInstance();
    if (isClipboardFilesBeCut()) {
        qDebug()<<uris;
        auto moveOp = new FileMoveOperation(uris, targetDirUri);
        moveOp->setAction(Qt::TargetMoveAction);
        op = moveOp;
        fileOpMgr->startOperation(moveOp, true);
        QApplication::clipboard()->clear();
    } else {

        qDebug() << "clipboard:" << uris;
        auto copyOp = new FileCopyOperation(uris, targetDirUri);
        op = copyOp;
        fileOpMgr->startOperation(copyOp, true);
    }

    if (m_target_directory_uri.size() <= 0 || m_target_directory_uri.back() != targetDirUri) {
        m_target_directory_uri.append(targetDirUri);
    }


    if (m_target_directory_uri.size() > 2) {
        m_target_directory_uri.pop_front();
    }

    return op;
}

void ClipboardUtils::clearClipboard()
{
    m_is_desktop_cut = false;
    m_is_peony_cut = false;
    QApplication::clipboard()->clear();
}

void ClipboardUtils::popLastTargetDirectoryUri(QString &uri)
{
    if (m_target_directory_uri.size() > 0 && uri == m_target_directory_uri.back()) {
        m_target_directory_uri.pop_back();
    }
}
