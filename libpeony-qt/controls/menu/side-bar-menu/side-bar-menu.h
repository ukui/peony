#ifndef SIDEBARMENU_H
#define SIDEBARMENU_H

#include <QMenu>
#include "peony-core_global.h"

namespace Peony {

class SideBarAbstractItem;
class SideBar;

class PEONYCORESHARED_EXPORT SideBarMenu : public QMenu
{
    Q_OBJECT
public:
    explicit SideBarMenu(SideBarAbstractItem *item, SideBar *sideBar, QWidget *parent = nullptr);

protected:
    const QList<QAction *> constructFavoriteActions();
    const QList<QAction *> constructPersonalActions();
    const QList<QAction *> constructFileSystemItemActions();

private:
    SideBarAbstractItem *m_item;
    SideBar *m_side_bar;
    QString m_uri;
};

}

#endif // SIDEBARMENU_H
