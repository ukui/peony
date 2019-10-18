#ifndef SEARCHVFSURIPARSER_H
#define SEARCHVFSURIPARSER_H

#include <QString>

namespace Peony {

class SearchVFSUriParser
{
public:
    const static QString parseSearchKey(const QString &uri, const QString &key);
    const static QString getSearchUriNameRegexp(const QString &searchUri);
    const static QString getSearchUriTargetDirectory(const QString &searchUri);
private:
    SearchVFSUriParser();
};

}

#endif // SEARCHVFSURIPARSER_H
