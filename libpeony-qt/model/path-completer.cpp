#include "path-completer.h"
#include "path-bar-model.h"

#include <QUrl>

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
    QUrl url = path;
    if (path.endsWith("/")) {
        model->setRootUri(path);
    } else {
        if (url.isValid()) {
            QString dirUri = url.toString();
            QString fileName = url.fileName();
            dirUri.remove(fileName);
            model->setRootUri(dirUri);
        }
    }

    return QCompleter::splitPath(path);
}

QString PathCompleter::pathFromIndex(const QModelIndex &index) const
{
    qDebug()<<index.data();
    return QCompleter::pathFromIndex(index);
}
