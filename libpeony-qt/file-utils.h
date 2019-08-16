#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "peony-core_global.h"
#include "gobject-template.h"

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

    static QString getNonSuffixedBaseNameFromUri(const QString &uri);

private:
    FileUtils();
};

}

#endif // FILEUTILS_H
