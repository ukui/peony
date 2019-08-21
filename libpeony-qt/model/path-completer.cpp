#include "path-completer.h"
#include "path-bar-model.h"

#include <QDebug>

using namespace Peony;

PathCompleter::PathCompleter(QObject *parent) : QCompleter(parent)
{

}

PathCompleter::PathCompleter(QAbstractItemModel *model, QObject *parent) : QCompleter(parent)
{
    Q_UNUSED(model);
}

QStringList PathCompleter::splitPath(const QString &path) const
{
    QAbstractItemModel *m = model();
    PathBarModel* model = static_cast<PathBarModel*>(m);
    if (path.endsWith("/")) {
        model->setRootUri(path);
    } else {
        QString tmp0 = path;
        QString tmp = path;
        tmp.chop(path.size() - path.lastIndexOf("/"));
        if (tmp.endsWith("/")) {
            tmp.append("/");
        }
        model->setRootUri(tmp);
    }

    return QCompleter::splitPath(path);
}

QString PathCompleter::pathFromIndex(const QModelIndex &index) const
{
    //qDebug()<<index.data();
    return QCompleter::pathFromIndex(index);
}
