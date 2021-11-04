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

#include "file-node.h"
#include "file-utils.h"
#include "file-info.h"
#include "file-node-reporter.h"

#include <QUrl>

using namespace Peony;

FileNode::FileNode(QString uri, FileNode *parent, FileNodeReporter *reporter)
{
    m_uri = uri;
    m_parent = parent;
    m_reporter = reporter;
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    char *basename = g_file_get_basename(file);
    m_basename = basename;
    m_dest_basename = basename;
    m_basename =  FileUtils::urlDecode(m_uri).split("/").last();
    m_dest_basename = m_basename;
    g_free(basename);

    //use G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS to avoid unnecessary recursion.
    m_is_folder = g_file_query_file_type(file, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr) == G_FILE_TYPE_DIRECTORY;
    GFileInfo *info = g_file_query_info(file,
                                        G_FILE_ATTRIBUTE_STANDARD_SIZE,
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);
    m_size = g_file_info_get_size(info);
    if (uri == "file:///proc/kcore")
        m_size = 0;
    g_object_unref(info);

    if (m_reporter) {
        m_reporter->sendNodeFound(m_uri, m_size);
    }

    m_children = new QList<FileNode*>();
}

FileNode::~FileNode() {
    qDebug()<<"delete node:"<<m_uri;
    m_uri.clear();
    m_basename.clear();
    m_dest_uri.clear();

    for (auto child : *m_children) {
        delete child;
    }
    m_children->clear();
    delete m_children;
}

void FileNode::findChildrenRecursively()
{
    if (m_reporter) {
        if (m_reporter->isOperationCancelled())
            return;
    }

    if (!m_is_folder)
        return;
    else {
        auto uris = FileUtils::getChildrenUris(m_uri);
        for (auto uri: uris) {
            FileNode *node = new FileNode(uri, this, m_reporter);
            m_children->append(node);
            node->findChildrenRecursively();
        }
    }
}

void FileNode::computeTotalSize(goffset *offset)
{
    *offset += m_size;
    for (auto child : *m_children) {
        child->computeTotalSize(offset);
    }
}

QString FileNode::getRelativePath()
{
    FileNode *n = this;
    while (n->m_parent) {
        n = n->m_parent;
    }
    GFile *root_file = g_file_new_for_uri(n->m_uri.toUtf8().constData());
    GFile *root_file_parent = g_file_get_parent(root_file);
    GFile *this_file = g_file_new_for_uri(m_uri.toUtf8().constData());

    char *relative_path = g_file_get_relative_path(root_file_parent, this_file);
    QString relativePath = relative_path;

    g_free(relative_path);
    g_object_unref(root_file);
    g_object_unref(root_file_parent);
    g_object_unref(this_file);

    return relativePath;
}

const QString FileNode::resolveDestFileUri(const QString &destRootDir)
{
    QStringList relativePathList;
    relativePathList.prepend(m_dest_basename);
    FileNode *parent = this->parent();
    while (parent) {
        relativePathList.prepend(parent->m_dest_basename);
        parent = parent->parent();
    }
    QString relativePath = relativePathList.join("/");
    if (relativePath.endsWith("/")) {
        relativePath.chop(1);
    }
    QString url = FileUtils::urlEncode(destRootDir + "/" + relativePath);
    setDestUri(url);
    return url;
}
