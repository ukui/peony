/*
 *  Copyright © 2002 Christophe Fergeau
 *  Copyright © 2003 Marco Pesenti Gritti
 *  Copyright © 2003, 2004 Christian Persch
 *    (ephy-notebook.c)
 *
 *  Copyright © 2008 Free Software Foundation, Inc.
 *    (peony-notebook.c)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  $Id: peony-notebook.h 8210 2008-04-11 20:05:25Z chpe $
 */

#ifndef PEONY_NOTEBOOK_H
#define PEONY_NOTEBOOK_H

#include <glib.h>

#include <gtk/gtk.h>
#include "peony-window-slot.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PEONY_TYPE_NOTEBOOK		(peony_notebook_get_type ())
#define PEONY_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_NOTEBOOK, PeonyNotebook))
#define PEONY_NOTEBOOK_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_NOTEBOOK, PeonyNotebookClass))
#define PEONY_IS_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_NOTEBOOK))
#define PEONY_IS_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_NOTEBOOK))
#define PEONY_NOTEBOOK_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_NOTEBOOK, PeonyNotebookClass))

    typedef struct _PeonyNotebookClass	PeonyNotebookClass;
    typedef struct _PeonyNotebook		PeonyNotebook;
    typedef struct _PeonyNotebookPrivate	PeonyNotebookPrivate;

    struct _PeonyNotebook
    {
        GtkNotebook parent;

        /*< private >*/
        PeonyNotebookPrivate *priv;
    };

    struct _PeonyNotebookClass
    {
        GtkNotebookClass parent_class;

        /* Signals */
        void	 (* tab_close_request)  (PeonyNotebook *notebook,
                                         PeonyWindowSlot *slot);
    };

    GType		peony_notebook_get_type		(void);

    int		peony_notebook_add_tab	(PeonyNotebook *nb,
                                     PeonyWindowSlot *slot,
                                     int position,
                                     gboolean jump_to);

    void		peony_notebook_set_show_tabs	(PeonyNotebook *nb,
            gboolean show_tabs);

    void		peony_notebook_set_dnd_enabled (PeonyNotebook *nb,
            gboolean enabled);
    void		peony_notebook_sync_tab_label (PeonyNotebook *nb,
            PeonyWindowSlot *slot);
    void		peony_notebook_sync_loading   (PeonyNotebook *nb,
            PeonyWindowSlot *slot);

    void		peony_notebook_reorder_current_child_relative (PeonyNotebook *notebook,
            int offset);
    void		peony_notebook_set_current_page_relative (PeonyNotebook *notebook,
            int offset);

    gboolean        peony_notebook_can_reorder_current_child_relative (PeonyNotebook *notebook,
            int offset);
    gboolean        peony_notebook_can_set_current_page_relative (PeonyNotebook *notebook,
            int offset);

#ifdef __cplusplus
}
#endif

#endif /* PEONY_NOTEBOOK_H */

