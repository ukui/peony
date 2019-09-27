#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QTreeView>
#include "peony-core_global.h"

namespace Peony {

class PEONYCORESHARED_EXPORT SideBar : public QTreeView
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = nullptr);

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri);

};

}

#endif // SIDEBAR_H
