#include "file-node.h"
#include "file-enumerator.h"
#include "file-info.h"
#include "file-node-reporter.h"

using namespace Peony;

FileNode::FileNode(QString uri, FileNode *parent, FileNodeReporter *reporter)
{
    m_uri = uri;
    m_parent = parent;
    m_reporter = reporter;
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    char *basename = g_file_get_basename(file);
    m_basename = basename;
    g_free(basename);

    //use G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS to avoid unnecessary recursion.
    m_is_folder = g_file_query_file_type(file, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr) == G_FILE_TYPE_DIRECTORY;
    GFileInfo *info = g_file_query_info(file,
                                        G_FILE_ATTRIBUTE_STANDARD_SIZE,
                                        G_FILE_QUERY_INFO_NONE,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);
    m_size = g_file_info_get_size(info);
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
        FileEnumerator e;
        e.setEnumerateDirectory(m_uri);
        e.enumerateSync();
        auto infos = e.getChildren();
        for (auto info: infos) {
            FileNode *node = new FileNode(info->uri(), this, m_reporter);
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
