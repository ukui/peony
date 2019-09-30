#ifndef TABPAGE_H
#define TABPAGE_H

#include <QTabWidget>
#include "peony-core_global.h"

namespace Peony {

class DirectoryViewContainer;

class PEONYCORESHARED_EXPORT TabPage : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabPage(QWidget *parent = nullptr);

    DirectoryViewContainer *getActivePage();

Q_SIGNALS:
    void currentActiveViewChanged();

    void updateWindowLocationRequest(const QString &uri, bool addHistory = true);

public Q_SLOTS:
    void addPage(const QString &uri);

protected:
    void rebindContainer();
};

}

#endif // TABPAGE_H
