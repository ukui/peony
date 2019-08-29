#ifndef PEONYSEARCHVFSFILE_H
#define PEONYSEARCHVFSFILE_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define PEONY_TYPE_SEARCH_VFS_FILE peony_search_vfs_file_get_type()

G_DECLARE_FINAL_TYPE(PeonySearchVFSFile, peony_search_vfs_file,
                     PEONY, SEARCH_VFS_FILE, GObject)

PeonySearchVFSFile *peony_search_vfs_file_new(void);

typedef struct {
    gchar *uri;
} PeonySearchVFSFilePrivate;


struct _PeonySearchVFSFile
{
    GObject parent_instance;

    PeonySearchVFSFilePrivate *priv;
};

G_END_DECLS

extern "C" {
    GFile *peony_search_vfs_file_new_for_uri(const char *uri);
    static GFileEnumerator *peony_search_vfs_file_enumerate_children_internal(GFile *file,
                                                                              const char *attribute,
                                                                              GFileQueryInfoFlags flags,
                                                                              GCancellable *cancellable,
                                                                              GError **error);
}

#endif // PEONYSEARCHVFSFILE_H
