/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* PeonyUndoStackManager - Manages undo of file operations (header)
 *
 * Copyright (C) 2007-2010 Amos Brocco
 * Copyright (C) 2011 Stefano Karapetsas
 *
 * Authors: Amos Brocco <amos.brocco@unifr.ch>,
 *          Stefano Karapetsas <stefano@karapetsas.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef PEONY_UNDOSTACK_MANAGER_H
#define PEONY_UNDOSTACK_MANAGER_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

/* Begin action structures */

typedef enum
{
  PEONY_UNDOSTACK_COPY,
  PEONY_UNDOSTACK_DUPLICATE,
  PEONY_UNDOSTACK_MOVE,
  PEONY_UNDOSTACK_RENAME,
  PEONY_UNDOSTACK_CREATEEMPTYFILE,
  PEONY_UNDOSTACK_CREATEFILEFROMTEMPLATE,
  PEONY_UNDOSTACK_CREATEFOLDER,
  PEONY_UNDOSTACK_MOVETOTRASH,
  PEONY_UNDOSTACK_CREATELINK,
  PEONY_UNDOSTACK_DELETE,
  PEONY_UNDOSTACK_RESTOREFROMTRASH,
  PEONY_UNDOSTACK_SETPERMISSIONS,
  PEONY_UNDOSTACK_RECURSIVESETPERMISSIONS,
  PEONY_UNDOSTACK_CHANGEOWNER,
  PEONY_UNDOSTACK_CHANGEGROUP
} PeonyUndoStackActionType;

typedef struct _PeonyUndoStackActionData PeonyUndoStackActionData;

typedef struct _PeonyUndoStackMenuData PeonyUndoStackMenuData;

struct _PeonyUndoStackMenuData {
  char* undo_label;
  char* undo_description;
  char* redo_label;
  char* redo_description;
};

/* End action structures */

typedef void
(*PeonyUndostackFinishCallback)(gpointer data);

typedef struct _PeonyUndoStackManagerPrivate PeonyUndoStackManagerPrivate;

typedef struct _PeonyUndoStackManager
{
  GObject parent_instance;

  PeonyUndoStackManagerPrivate* priv;

} PeonyUndoStackManager;

typedef struct _PeonyUndoStackManagerClass
{
  GObjectClass parent_class;
  
} PeonyUndoStackManagerClass;

#define TYPE_PEONY_UNDOSTACK_MANAGER (peony_undostack_manager_get_type())

#define PEONY_UNDOSTACK_MANAGER(object) \
 (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_PEONY_UNDOSTACK_MANAGER, PeonyUndoStackManager))

#define PEONY_UNDOSTACK_MANAGER_CLASS(klass) \
 (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_PEONY_UNDOSTACK_MANAGER, PeonyUndoStackManagerClass))

#define IS_PEONY_UNDOSTACK_MANAGER(object) \
 (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_PEONY_UNDOSTACK_MANAGER))

#define IS_PEONY_UNDOSTACK_MANAGER_CLASS(klass) \
 (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_PEONY_UNDOSTACK_MANAGER))

#define PEONY_UNDOSTACK_MANAGER_GET_CLASS(object) \
 (G_TYPE_INSTANCE_GET_CLASS((object), TYPE_PEONY_UNDOSTACK_MANAGER, PeonyUndoStackManagerClass))
 
GType
peony_undostack_manager_get_type (void);

void
peony_undostack_manager_add_action(PeonyUndoStackManager* manager,
    PeonyUndoStackActionData* action);

void
peony_undostack_manager_undo(PeonyUndoStackManager* manager,
    GtkWidget *parent_view, PeonyUndostackFinishCallback cb);

void
peony_undostack_manager_redo(PeonyUndoStackManager* manager,
    GtkWidget *parent_view, PeonyUndostackFinishCallback cb);

PeonyUndoStackActionData*
peony_undostack_manager_data_new(PeonyUndoStackActionType type,
    gint items_count);

gboolean
peony_undostack_manager_is_undo_redo(PeonyUndoStackManager* manager);

void
peony_undostack_manager_trash_has_emptied(PeonyUndoStackManager* manager);

PeonyUndoStackManager*
peony_undostack_manager_instance(void);

void
peony_undostack_manager_data_set_src_dir(PeonyUndoStackActionData* data,
    GFile* src);

void
peony_undostack_manager_data_set_dest_dir(PeonyUndoStackActionData* data,
    GFile* dest);

void
peony_undostack_manager_data_add_origin_target_pair(
    PeonyUndoStackActionData* data, GFile* origin, GFile* target);

void
peony_undostack_manager_data_set_create_data(
    PeonyUndoStackActionData* data, char* target_uri, char* template_uri);

void
peony_undostack_manager_data_set_rename_information(
    PeonyUndoStackActionData* data, GFile* old_file, GFile* new_file);

guint64
peony_undostack_manager_get_file_modification_time(GFile* file);

void
peony_undostack_manager_data_add_trashed_file(
    PeonyUndoStackActionData* data, GFile* file, guint64 mtime);

void
peony_undostack_manager_request_menu_update(PeonyUndoStackManager* manager);

void
peony_undostack_manager_data_add_file_permissions(
    PeonyUndoStackActionData* data, GFile* file, guint32 permission);
    
void
peony_undostack_manager_data_set_recursive_permissions(
    PeonyUndoStackActionData* data, guint32 file_permissions, guint32 file_mask,
	guint32 dir_permissions, guint32 dir_mask);
    
void
peony_undostack_manager_data_set_file_permissions(
    PeonyUndoStackActionData* data, char* uri, guint32 current_permissions, guint32 new_permissions);
    
void
peony_undostack_manager_data_set_owner_change_information(
    PeonyUndoStackActionData* data, char* uri, const char* current_user, const char* new_user);
    
void
peony_undostack_manager_data_set_group_change_information(
    PeonyUndoStackActionData* data, char* uri, const char* current_group, const char* new_group);    

#endif /* PEONY_UNDOSTACK_MANAGER_H */
