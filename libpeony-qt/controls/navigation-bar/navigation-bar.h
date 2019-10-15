#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QToolBar>
#include "peony-core_global.h"

namespace Peony {

class DirectoryViewContainer;

class AdvancedLocationBar;
class NavigationToolBar;

class PEONYCORESHARED_EXPORT NavigationBar : public QToolBar
{
    Q_OBJECT
public:
    explicit NavigationBar(QWidget *parent = nullptr);

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri, bool addHistory, bool forceUpdate = false);
    void refreshRequest();

public Q_SLOTS:
    void bindContainer(DirectoryViewContainer *container);
    void updateLocation(const QString &uri);

    void setBlock(bool block = true);

protected:
    const QString getCurrentUri();

private:
    NavigationToolBar *m_left_control;
    AdvancedLocationBar *m_center_control;
};

}

#endif // NAVIGATIONBAR_H
