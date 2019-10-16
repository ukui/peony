#ifndef DIRECTORYVIEWMENU_H
#define DIRECTORYVIEWMENU_H

#include <QMenu>
#include "peony-core_global.h"

#include "directory-view-plugin-iface.h"

namespace Peony {

class PEONYCORESHARED_EXPORT DirectoryViewMenu : public QMenu
{
    Q_OBJECT
public:
    explicit DirectoryViewMenu(DirectoryViewIface *directoryView, QWidget *parent = nullptr);

protected:
    const QList<QAction *> constructOpenOpActions();
    const QList<QAction *> constructViewOpActions();
    const QList<QAction *> constructFileOpActions();
    const QList<QAction *> constructMenuPluginActions(); //directory view menu extension.
    const QList<QAction *> constructFilePropertiesActions();
    const QList<QAction *> constructComputerActions();
    const QList<QAction *> constructTrashActions();
    const QList<QAction *> constructSearchActions();

private:
    DirectoryViewIface *m_view;
    QString m_directory;
    QStringList m_selections;

    bool m_is_computer = false;
    bool m_is_trash = false;
    bool m_is_search = false;
};

}

#endif // DIRECTORYVIEWMENU_H
