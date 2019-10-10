#ifndef SEARCHVFSURIPARSER_H
#define SEARCHVFSURIPARSER_H

#include <QString>

namespace Peony {

class SearchVFSUriParser
{
public:
    const static QString parseSearchKey(const QString &uri, const QString &key);
private:
    SearchVFSUriParser();
};

}

#endif // SEARCHVFSURIPARSER_H
