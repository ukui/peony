#include "peony-search-vfs-file.h"
#include "peony-search-vfs-file-enumerator.h"
#include "file-enumerator.h"
#include "search-vfs-manager.h"
#include <QString>
#include <QDebug>

/* -- GFileIface -- */
static void peony_search_vfs_file_g_file_iface_init(GFileIface *iface);

static GFile *peony_search_vfs_file_dup(GFile *file);
static guint peony_search_vfs_file_hash(GFile *file);//unused
static gboolean peony_search_vfs_file_equal(GFile *file1, GFile *file2);//unused
static gboolean peony_search_vfs_file_is_native(GFile *file);//unused
static gboolean peony_search_vfs_file_has_uri_scheme(GFile *file, const char *uri_scheme);//unused
static char *peony_search_vfs_file_get_uri_sheceme(GFile *file);//unused
static char *peony_search_vfs_file_get_basename(GFile *file);//unused
static char *peony_search_vfs_file_get_path(GFile *file);//unused
static char *peony_search_vfs_file_get_uri(GFile *file);
static char *peony_search_vfs_file_get_parse_name(GFile *file);//unused
static GFile *peony_search_vfs_file_get_parent(GFile *file);//unused
static gboolean peony_search_vfs_file_prefix_matches(GFile *prefix, GFile *file);//unused
static char *peony_search_vfs_file_get_relative_path(GFile *file, const char *relative_path);//unused
static GFile *peony_search_vfs_file_resolve_relative_path(GFile *file,
                                                          const char *relative_path);
static GFile *peony_search_vfs_file_get_child_for_display_name (GFile *file,
                                                                const char *display_name,
                                                                GError **err);//unused

static GFileInfo *peony_search_vfs_file_query_info(GFile *file,
                                                   const char *attributes,
                                                   GFileQueryInfoFlags flags,
                                                   GCancellable *cancellable,
                                                   GError **error);

/* -- Implement Iface -- */
G_DEFINE_TYPE_EXTENDED(PeonySearchVFSFile,
                       peony_search_vfs_file,
                       G_TYPE_OBJECT,
                       0,
                       G_ADD_PRIVATE(PeonySearchVFSFile)
                       G_IMPLEMENT_INTERFACE(G_TYPE_FILE, peony_search_vfs_file_g_file_iface_init));

static void file_dispose(GObject *object)
{
    auto vfs_file = PEONY_SEARCH_VFS_FILE(object);
    if (vfs_file->priv->uri) {
        g_free(vfs_file->priv->uri);
    }
}

static void peony_search_vfs_file_class_init (PeonySearchVFSFileClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = file_dispose;
}

GFileEnumerator *peony_search_vfs_file_enumerate_children(GFile *file,
                                                          const char *attribute,
                                                          GFileQueryInfoFlags flags,
                                                          GCancellable *cancellable,
                                                          GError **error)
{
    auto search_vfs_file = PEONY_SEARCH_VFS_FILE(file);

    return peony_search_vfs_file_enumerate_children_internal(file, attribute, flags, cancellable, error);
}

GFileInfo *peony_search_vfs_file_query_info(GFile *file,
                                            const char *attributes,
                                            GFileQueryInfoFlags flags,
                                            GCancellable *cancellable,
                                            GError **error)
{
    auto vfs_file = PEONY_SEARCH_VFS_FILE(file);
    GFileInfo *info = g_file_info_new();
    g_file_info_set_name(info, vfs_file->priv->uri);
    auto icon = g_themed_icon_new("search");
    g_file_info_set_icon(info, icon);
    g_object_unref(icon);
    g_file_info_set_display_name(info, "");
    g_file_info_set_file_type(info, G_FILE_TYPE_DIRECTORY);
    return info;
}


static void peony_search_vfs_file_init(PeonySearchVFSFile *self)
{
    PeonySearchVFSFilePrivate *priv = (PeonySearchVFSFilePrivate*)peony_search_vfs_file_get_instance_private(self);
    self->priv = priv;
    priv->uri = nullptr;
}

char *peony_search_vfs_file_get_uri(GFile *file)
{
    auto vfs_file = PEONY_SEARCH_VFS_FILE(file);
    return g_strdup(vfs_file->priv->uri);
}

GFile *peony_search_vfs_file_get_parent(GFile *file)
{
    Q_UNUSED(file);
    return nullptr;
}

gboolean peony_search_vfs_file_is_native(GFile *file)
{
    return false;
}

static void peony_search_vfs_file_g_file_iface_init(GFileIface *iface)
{
    iface->dup = peony_search_vfs_file_dup;
    iface->get_parent = peony_search_vfs_file_get_parent;
    iface->is_native = peony_search_vfs_file_is_native;
    iface->enumerate_children = peony_search_vfs_file_enumerate_children;
    iface->query_info = peony_search_vfs_file_query_info;
    iface->get_uri = peony_search_vfs_file_get_uri;
    iface->resolve_relative_path = peony_search_vfs_file_resolve_relative_path;
}

/// dup, get_parent and is_native(and so on) is used in peony-qt,
/// such as FileEnumerator and FileInfo, so i must need
/// implement these search vfs file's functions.
GFile *peony_search_vfs_file_dup(GFile *file)
{
    if (!PEONY_IS_SEARCH_VFS_FILE(file)) {
        //this should not be happend.
        return g_file_new_for_uri("search:///");
    }
    auto vfs_file = PEONY_SEARCH_VFS_FILE(file);
    auto dup = PEONY_SEARCH_VFS_FILE(g_object_new(PEONY_TYPE_SEARCH_VFS_FILE, nullptr));
    dup->priv->uri = g_strdup(vfs_file->priv->uri);
    return G_FILE(dup);
}

GFile *peony_search_vfs_file_resolve_relative_path(GFile *file, const char *relative_path)
{
    Q_UNUSED(file);
    //FIXME: maybe i should put the resolve to vfs look up method?
    QString tmp = relative_path;
    if (tmp.contains("real-uri:")) {
        tmp = tmp.remove("real-uri:");
        return g_file_new_for_uri(tmp.toUtf8().constData());
    }
    return g_file_new_for_uri("serarch:///");
}

GFile *peony_search_vfs_file_new_for_uri(const char *uri)
{
    auto search_vfs_file = PEONY_SEARCH_VFS_FILE(g_object_new(PEONY_TYPE_SEARCH_VFS_FILE, nullptr));
    search_vfs_file->priv->uri = g_strdup(uri);

    return G_FILE(search_vfs_file);
}

void peony_search_vfs_file_enumerator_parse_uri(PeonySearchVFSFileEnumerator *enumerator,
                                                const char *uri)
{
    PeonySearchVFSFileEnumeratorPrivate *details = enumerator->priv;

    *details->search_vfs_directory_uri = uri;

    auto manager = Peony::SearchVFSManager::getInstance();
    if (manager->hasHistory(uri)) {
        auto uris = manager->getHistroyResults(uri);
        for (auto uri: uris) {
            details->enumerate_queue->enqueue(Peony::FileInfo::fromUri(uri));
        }
        //do not parse uri, not neccersary
        return;
    }

    QStringList args = details->search_vfs_directory_uri->split("&", QString::SkipEmptyParts);

    //we should judge case sensitive, then we confirm the regexp when
    //we match file in file enumeration.
    for (auto arg: args) {
        //qDebug()<<arg;
        if (arg.contains("search_hidden=")) {
            if (arg.endsWith("1")) {
                details->search_hidden = true;
            }
            continue;
        }
        if (arg.contains("use_regexp=")) {
            if (arg.endsWith("0")) {
                details->use_regexp = false;
            }
            continue;
        }

        if (arg.contains("case_sensitive=")) {
            if (arg.endsWith("1")) {
                details->case_sensitive = false;
            }
            continue;
        }

        if (arg.contains("name_regexp=")) {
            if (arg == "name_regexp=") {
                continue;
            }
            QString tmp = arg;
            tmp = tmp.remove("name_regexp=");
            details->name_regexp = new QRegExp(tmp);
            continue;
        }

        if (arg.contains("content_regexp=")) {
            if (arg == "content_regexp=") {
                continue;
            }
            QString tmp = arg;
            tmp = tmp.remove("content_regexp=");
            details->content_regexp = new QRegExp(tmp);
            continue;
        }

        if (arg.contains("save=")) {
            if (arg.endsWith("1")) {
                details->save_result = true;
            }
            continue;
        }

        if (arg.contains("recursive=")) {
            if (arg.endsWith("1")) {
                details->recursive = true;
            }
            continue;
        }

        if (arg.contains("search_uris=")) {
            QString tmp = arg;
            tmp.remove("search:///");
            tmp.remove("search_uris=");
            QStringList uris = tmp.split(",", QString::SkipEmptyParts);
            for (auto uri: uris) {
                //NOTE: we should enumerate the search uris and add
                //the children into queue first. otherwise we could
                //not judge wether we should search recursively.
                Peony::FileEnumerator e;
                e.setEnumerateDirectory(uri);
                e.enumerateSync();
                auto infos = e.getChildren();
                for (auto info : infos) {
                    details->enumerate_queue->enqueue(info);
                }
            }
        }
    }

    Qt::CaseSensitivity sensitivity = details->case_sensitive? Qt::CaseSensitive: Qt::CaseInsensitive;

    if (!details->name_regexp) {
        //details->name_regexp = new QRegExp;
    } else {
        details->name_regexp->setCaseSensitivity(sensitivity);
    }

    if (!details->content_regexp) {
        //details->content_regexp = new QRegExp;
    } else {
        details->content_regexp->setCaseSensitivity(sensitivity);
    }
}

GFileEnumerator *peony_search_vfs_file_enumerate_children_internal(GFile *file,
                                                                   const char *attributes,
                                                                   GFileQueryInfoFlags flags,
                                                                   GCancellable *cancellable,
                                                                   GError **error)
{
    auto vfs_file = PEONY_SEARCH_VFS_FILE(file);

    //we should add enumerator container when search vfs enumerator created.
    //otherwise g_enumerator_get_child will went error.
    auto enumerator = PEONY_SEARCH_VFS_FILE_ENUMERATOR(g_object_new(PEONY_TYPE_SEARCH_VFS_FILE_ENUMERATOR,
                                                                    "container", file,
                                                                    nullptr));
    peony_search_vfs_file_enumerator_parse_uri(enumerator, vfs_file->priv->uri);
    //parse uri, add top folder uri to queue;
    return G_FILE_ENUMERATOR(enumerator);
}
