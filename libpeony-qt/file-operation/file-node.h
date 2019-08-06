#ifndef FILENODE_H
#define FILENODE_H

#include <QString>
#include <QList>
#include <gio/gio.h>
#include <memory>

namespace Peony {

class FileNodeReporter;

/*!
 * \brief The FileNode class
 * <br>
 * This class contains 3 parts:
 * 1. the self info, such as uri, basename, etc,
 * 2. the parent node and list of children,
 * 3. the reporter handle.
 * </br>
 * <br>
 * FileNode is smilar to FileItem, but it is more simple. It is used with many limitation.
 * Cause it method is Synchronized, you should never try use this class in the ui-thread.
 * This class is desgined for file operation classes which running in other threads.
 * For example, a copy operation might need enumerate all children for get the total size
 * of the source files.
 * </br>
 * <br>
 * The FileNodeReporter handle is used for send signal to tell other object the current state
 * of file node enumeration. Actually, a FileNode instance always be with a FileNodeReproter
 * instance at its initialization.
 * </br>
 * \see FileNodeReporter.
 */
class FileNode
{
    friend class FileNodeReporter;
public:
    FileNode(QString uri, FileNode* parent, FileNodeReporter *reporter = nullptr);
    ~FileNode();

    void findChildrenRecursively();
    void computeTotalSize(goffset *offset);

    QString uri() {return m_uri;}
    QString baseName() {return m_basename;}
    FileNode *parent() {return m_parent;}
    QList<FileNode*> *children() {return m_children;}
    qint64 size() {return m_size;}
    bool isFolder() {return m_is_folder;}

    QString getRelativePath();

private:
    QString m_uri = nullptr;
    QString m_basename = nullptr;
    goffset m_size = 0;
    bool m_is_folder = false;
    FileNode *m_parent = nullptr;
    QList<FileNode*> *m_children = nullptr;

    FileNodeReporter *m_reporter = nullptr;
};

}

#endif // FILENODE_H
