#ifndef PEONYSEARCHVFSFILEENUMERATOR_H
#define PEONYSEARCHVFSFILEENUMERATOR_H

#include <gio/gio.h>
#include <QQueue>
#include <QRegExp>
#include "file-info.h"

G_BEGIN_DECLS

#define PEONY_TYPE_SEARCH_VFS_FILE_ENUMERATOR peony_search_vfs_file_enumerator_get_type()
G_DECLARE_FINAL_TYPE(PeonySearchVFSFileEnumerator,
                     peony_search_vfs_file_enumerator,
                     PEONY, SEARCH_VFS_FILE_ENUMERATOR,
                     GFileEnumerator)

PeonySearchVFSFileEnumerator *peony_search_vfs_file_enumerator_new(void);

typedef struct {
    QString *search_vfs_directory_uri;
    gboolean search_hidden;
    gboolean use_regexp;
    gboolean save_result;
    gboolean recursive;
    gboolean case_sensitive;
    QRegExp *name_regexp;
    QRegExp *content_regexp;
    gboolean match_name_or_content;
    QQueue<std::shared_ptr<Peony::FileInfo>> *enumerate_queue;
} PeonySearchVFSFileEnumeratorPrivate;

struct _PeonySearchVFSFileEnumerator
{
    GFileEnumerator parent_instance;

    PeonySearchVFSFileEnumeratorPrivate *priv;
};

G_END_DECLS

#endif // PEONYSEARCHVFSFILEENUMERATOR_H
