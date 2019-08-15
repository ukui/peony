#include "file-utils.h"
#include <QUrl>
#include <QFileInfo>

using namespace Peony;

FileUtils::FileUtils()
{

}

QString FileUtils::getQStringFromCString(char *c_string, bool free)
{
    QString value = c_string;
    if (free)
        g_free(c_string);
    return value;
}

QString FileUtils::getFileUri(const GFileWrapperPtr &file)
{
    char *uri = g_file_get_uri(file.get()->get());
    return getQStringFromCString(uri);
}

QString FileUtils::getFileBaseName(const GFileWrapperPtr &file)
{
    char *basename = g_file_get_basename(file.get()->get());
    return getQStringFromCString(basename);
}

QString FileUtils::getUriBaseName(const QString &uri)
{
    QUrl url = uri;
    return url.fileName();
}

GFileWrapperPtr FileUtils::getFileParent(const GFileWrapperPtr &file)
{
    return wrapGFile(g_file_get_parent(file.get()->get()));
}

QString FileUtils::getRelativePath(const GFileWrapperPtr &dir, const GFileWrapperPtr &file)
{
    char *relative_path = g_file_get_relative_path(dir.get()->get(), file.get()->get());
    return getQStringFromCString(relative_path);
}

GFileWrapperPtr FileUtils::resolveRelativePath(const GFileWrapperPtr &dir, const QString &relativePath)
{
    return wrapGFile(g_file_resolve_relative_path(dir.get()->get(),
                                                  relativePath.toUtf8().constData()));
}

bool FileUtils::getFileHasChildren(const GFileWrapperPtr &file)
{
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_DIRECTORY || type == G_FILE_TYPE_MOUNTABLE;
}

bool FileUtils::getFileIsFolder(const GFileWrapperPtr &file)
{
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_DIRECTORY;
}

QString FileUtils::getNonSuffixedBaseNameFromUri(const QString &uri)
{
    QUrl url = uri;
    if (url.isLocalFile()) {
        QFileInfo info(url.path());
        return info.baseName();
    } else {
        QString suffixedBaseName = url.fileName();
        int index = suffixedBaseName.lastIndexOf(".");
        if (index != -1) {
            QString suffix = suffixedBaseName.chopped(suffixedBaseName.size() - index);
            if (suffix == ".gz" || suffix == ".xz" || suffix == ".bz"
                    || suffix == ".bz2" || suffix == ".Z" ||
                    suffix == ".sit") {
                int secondIndex = suffixedBaseName.lastIndexOf('.');
                suffixedBaseName.chop(suffixedBaseName.size() - secondIndex);
            }
        }
        return suffixedBaseName;
    }
}
