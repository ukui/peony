#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "peony-core_global.h"
#include "gobject-template.h"
#include "gerror-wrapper.h"

#include <QString>

namespace Peony {

class PEONYCORESHARED_EXPORT FileUtils
{
public:
    static QString getQStringFromCString(char *c_string, bool free = true);
    static QString getFileUri(const GFileWrapperPtr &file);
    static QString getFileBaseName(const GFileWrapperPtr &file);
    static QString getUriBaseName(const QString &uri);
    static GFileWrapperPtr getFileParent(const GFileWrapperPtr &file);
    static QString getRelativePath(const GFileWrapperPtr &dir, const GFileWrapperPtr &file);
    static GFileWrapperPtr resolveRelativePath(const GFileWrapperPtr &dir, const QString &relativePath);
    static bool getFileHasChildren(const GFileWrapperPtr &file);
    static bool getFileIsFolder(const GFileWrapperPtr &file);
    static bool getFileIsFolder(const QString &uri);

    static QString getNonSuffixedBaseNameFromUri(const QString &uri);
    static QString getFileDisplayName(const QString &uri);
    static QString getFileIconName(const QString &uri);

    static GErrorWrapperPtr getEnumerateError(const QString &uri);
    static QString getTargetUri(const QString &uri);

    static bool stringStartWithChinese(const QString &string);
    static bool stringLesserThan(const QString &left, const QString &right);

    static const QString getParentUri(const QString &uri);

    static bool isFileExsit(const QString &uri);

private:
    FileUtils();
};

}

#endif // FILEUTILS_H
