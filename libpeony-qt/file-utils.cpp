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
    QUrl url = QString(uri);
    g_free(uri);
    return url.toDisplayString();
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

bool FileUtils::getFileIsFolder(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
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

QString FileUtils::getFileDisplayName(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                                G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                                                G_FILE_QUERY_INFO_NONE,
                                                nullptr,
                                                nullptr));
    return g_file_info_get_display_name(info.get()->get());
}

QString FileUtils::getFileIconName(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                                G_FILE_ATTRIBUTE_STANDARD_ICON,
                                                G_FILE_QUERY_INFO_NONE,
                                                nullptr,
                                                nullptr));
    GIcon *g_icon = g_file_info_get_icon (info.get()->get());
    QString icon_name;
    //do not unref the GIcon from info.
    if (G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        if (icon_names)
            icon_name = QString (*icon_names);
    }
    return icon_name;
}

GErrorWrapperPtr FileUtils::getEnumerateError(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    GError *err = nullptr;
    auto enumerator = wrapGFileEnumerator(g_file_enumerate_children(file.get()->get(),
                                                                    G_FILE_ATTRIBUTE_STANDARD_NAME,
                                                                    G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                                    nullptr,
                                                                    &err));
    if (err) {
        return GErrorWrapper::wrapFrom(err);
    }
    return nullptr;
}

QString FileUtils::getTargetUri(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                                G_FILE_ATTRIBUTE_STANDARD_TARGET_URI,
                                                G_FILE_QUERY_INFO_NONE,
                                                nullptr,
                                                nullptr));
    return g_file_info_get_attribute_string(info.get()->get(),
                                            G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
}

bool FileUtils::stringStartWithChinese(const QString &string)
{
    if (string.isEmpty())
        return false;

    auto firstStrUnicode = string.at(0).unicode();
    return (firstStrUnicode <=0x9FA5 && firstStrUnicode >= 0x4E00);
}

bool FileUtils::stringLesserThan(const QString &left, const QString &right)
{
    bool leftStartWithChinese = stringStartWithChinese(left);
    bool rightStartWithChinese = stringStartWithChinese(right);
    if (!(!leftStartWithChinese && !rightStartWithChinese)) {
        return leftStartWithChinese;
    }
    return left.toLower() < right.toLower();
}

const QString FileUtils::getParentUri(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto parent = getFileParent(file);
    auto parentUri = getFileUri(parent);
    return parentUri == uri? nullptr: parentUri;
}
