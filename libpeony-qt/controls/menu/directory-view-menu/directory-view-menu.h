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
};

}

#endif // DIRECTORYVIEWMENU_H
