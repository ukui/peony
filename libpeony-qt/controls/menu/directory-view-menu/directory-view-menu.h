/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#ifndef DIRECTORYVIEWMENU_H
#define DIRECTORYVIEWMENU_H

#include <QMenu>
#include "peony-core_global.h"

#include "directory-view-plugin-iface2.h"

namespace Peony {

class FMWindow;

/*!
 * \brief The DirectoryViewMenu class
 *
 * \todo
 * add create template function.
 */
class PEONYCORESHARED_EXPORT DirectoryViewMenu : public QMenu
{
    Q_OBJECT
public:
    /*!
     * \brief DirectoryViewMenu
     * \param directoryView
     * \param parent
     * \deprecated
     */
    explicit DirectoryViewMenu(DirectoryViewWidget *directoryView, QWidget *parent = nullptr);
    explicit DirectoryViewMenu(FMWindow *window, QWidget *parent = nullptr);

    const QStringList &urisToEdit() {return m_uris_to_edit;}

protected:
    void fillActions();
    const QList<QAction *> constructOpenOpActions();
    const QList<QAction *> constructCreateTemplateActions();
    const QList<QAction *> constructViewOpActions();
    const QList<QAction *> constructFileOpActions();
    const QList<QAction *> constructMenuPluginActions(); //directory view menu extension.
    const QList<QAction *> constructFilePropertiesActions();
    const QList<QAction *> constructComputerActions();
    const QList<QAction *> constructTrashActions();
    const QList<QAction *> constructSearchActions();

private:
    FMWindow *m_top_window;

    DirectoryViewWidget *m_view;
    QString m_directory;
    QStringList m_selections;

    bool m_is_computer = false;
    bool m_is_trash = false;
    bool m_is_search = false;

    QStringList m_uris_to_edit;
};

}

#endif // DIRECTORYVIEWMENU_H
