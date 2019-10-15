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
    void currentLocationChanged();
    void currentSelectionChanged();

    void updateWindowLocationRequest(const QString &uri, bool addHistory = true, bool forceUpdate = false);

public Q_SLOTS:
    void addPage(const QString &uri);
    void refreshCurrentTabText();

    void stopLocationChange();

protected:
    void rebindContainer();
};

}

#endif // TABPAGE_H
