#include "search-vfs-uri-parser.h"

using namespace Peony;

SearchVFSUriParser::SearchVFSUriParser()
{

}

const QString SearchVFSUriParser::parseSearchKey(const QString &uri, const QString &key)
{
    return QString("search:///search_uris="+uri+"&name_regexp="+key+"&recursive=1");
}
