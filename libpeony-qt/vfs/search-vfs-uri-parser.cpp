#include "search-vfs-uri-parser.h"
#include "file-utils.h"

#include <QStringList>
#include <QDebug>
using namespace Peony;

SearchVFSUriParser::SearchVFSUriParser()
{

}

const QString SearchVFSUriParser::parseSearchKey(const QString &uri, const QString &key)
{
    return QString("search:///search_uris="+uri+"&name_regexp="+key+"&recursive=1");
}

const QString SearchVFSUriParser::getSearchUriNameRegexp(const QString &searchUri)
{
    auto string = searchUri;
    string.remove("search:///");
    auto list = string.split("&");
    for (auto arg : list) {
        if (arg.startsWith("name_regexp=")) {
            qDebug()<<arg;
            auto tmp = arg.remove("name_regexp=");
            return tmp;
        }
    }
    return nullptr;
}

const QString SearchVFSUriParser::getSearchUriTargetDirectory(const QString &searchUri)
{
    auto string = searchUri;
    string.remove("search:///");
    auto list = string.split("&");
    for (auto arg : list) {
        if (arg.startsWith("search_uris=")) {
            qDebug()<<arg;
            auto tmp = arg.remove("search_uris=");
            auto uris = tmp.split(",");
            if (uris.count() == 1) {
                return FileUtils::getFileDisplayName(tmp);
            }
            tmp = nullptr;
            QStringList names;
            for (auto uri : uris) {
                auto displayName = FileUtils::getFileDisplayName(uri);
                tmp = tmp + ", " + displayName;
            }
            return tmp;
        }
    }
    return nullptr;
}
