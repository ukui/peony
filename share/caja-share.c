/* caja-share -- Caja File Sharing Extension
 *
 * Sebastien Estienne <sebastien.estienne@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * (C) Copyright 2005 Ethium, Inc.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-column-provider.h>
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-file-info.h>
#include <libcaja-extension/caja-info-provider.h>
#include <libcaja-extension/caja-menu-provider.h>
#include <libcaja-extension/caja-property-page-provider.h>

#include "caja-share.h"

#include <glib/gi18n-lib.h>

#include <gio/gio.h>

#include <gtk/gtk.h>

#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "shares.h"


#define NEED_IF_GUESTOK_MASK (S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) /* need go+rx for guest enabled usershares */
#define NEED_IF_WRITABLE_MASK (S_IWGRP | S_IWOTH)			/* writable usershares need go+w additionally*/
#define NEED_ALL_MASK         (NEED_IF_GUESTOK_MASK | NEED_IF_WRITABLE_MASK)

static GObjectClass *parent_class;

/* Structure to hold all the information for a share's property page.  If
 * you add stuff to this, add it to free_property_page_cb() as well.
 */
typedef struct {
  char *path; /* Full path which is being shared */
  CajaFileInfo *fileinfo; /* Caja file to which this page refers */

  GtkBuilder *ui;

  GtkWidget *main; /* Widget that holds all the rest.  Its "PropertyPage" GObject-data points to this PropertyPage structure */
  
  GtkWidget *checkbutton_share_folder;
  GtkWidget *hbox_share_name;
  GtkWidget *hbox_share_comment;
  GtkWidget *entry_share_name;
  GtkWidget *checkbutton_share_rw_ro;
  GtkWidget *checkbutton_share_guest_ok;
  GtkWidget *entry_share_comment;
  GtkWidget *label_status;
  GtkWidget *button_cancel;
  GtkWidget *button_apply;

  GtkWidget *standalone_window;

  gboolean was_initially_shared;
  gboolean was_writable;
  gboolean is_dirty;
} PropertyPage;

static void property_page_set_warning (PropertyPage *page);
static void property_page_set_error (PropertyPage *page, const char *message);
static void property_page_set_normal (PropertyPage *page);

static void
property_page_validate_fields (PropertyPage *page)
{
  const char *name;

  name = gtk_entry_get_text (GTK_ENTRY (page->entry_share_name));
  
  if (g_utf8_strlen (name, -1) <= 12)
    property_page_set_normal (page);
  else
    property_page_set_warning (page);
}

static gboolean
message_confirm_missing_permissions (GtkWidget *widget, const char *path, mode_t need_mask)
{
  GtkWidget *toplevel;
  GtkWidget *dialog;
  char *display_name;
  gboolean result;

  toplevel = gtk_widget_get_toplevel (widget);
  if (!GTK_IS_WINDOW (toplevel))
    toplevel = NULL;

  display_name = g_filename_display_basename (path);

  dialog = gtk_message_dialog_new (toplevel ? GTK_WINDOW (toplevel) : NULL,
				   0,
				   GTK_MESSAGE_QUESTION,
				   GTK_BUTTONS_NONE,
				   _("Caja needs to add some permissions to your folder \"%s\" in order to share it"),
				   display_name);

  /* FIXME: the following message only mentions "permission by others".  We
   * should probably be more explicit and mention group/other permissions.
   * We'll be able to do that after the period of string freeze.
   */
  gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
					    _("The folder \"%s\" needs the following extra permissions for sharing to work:\n"
					      "%s%s%s"
					      "Do you want Caja to add these permissions to the folder automatically?"),
					    display_name,
					    (need_mask & (S_IRGRP | S_IROTH)) ? _("  - read permission by others\n") : "",
					    (need_mask & (S_IWGRP | S_IWOTH)) ? _("  - write permission by others\n") : "",
					    (need_mask & (S_IXGRP | S_IXOTH)) ? _("  - execute permission by others\n") : "");
  g_free (display_name);

  gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
  gtk_dialog_add_button (GTK_DIALOG (dialog), _("Add the permissions automatically"), GTK_RESPONSE_ACCEPT);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);

  result = gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT;
  gtk_widget_destroy (dialog);

  return result;
}

static void
error_when_changing_permissions (GtkWidget *widget, const char *path)
{
  GtkWidget *toplevel;
  GtkWidget *dialog;
  char *display_name;

  toplevel = gtk_widget_get_toplevel (widget);
  if (!GTK_IS_WINDOW (toplevel))
    toplevel = NULL;

  display_name = g_filename_display_basename (path);

  dialog = gtk_message_dialog_new (toplevel ? GTK_WINDOW (toplevel) : NULL,
				   0,
				   GTK_MESSAGE_ERROR,
				   GTK_BUTTONS_OK,
				   _("Could not change the permissions of folder \"%s\""),
				   display_name);
  g_free (display_name);

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static char *
get_key_file_path (void)
{
  return g_build_filename (g_get_home_dir (), ".mate2", "mate-file-manager-share-modified-permissions", NULL);
}

static void
save_key_file (const char *filename, GKeyFile *key_file)
{
  char *contents;
  gsize length;

  /* NULL GError */
  contents = g_key_file_to_data (key_file, &length, NULL);
  if (!contents)
    return;

  /* NULL GError */
  g_file_set_contents (filename, contents, length, NULL);

  g_free (contents);
}

static void
save_changed_permissions (const char *path, mode_t need_mask)
{
  GKeyFile *key_file;
  char *key_file_path;
  char str[50];

  key_file = g_key_file_new ();
  key_file_path = get_key_file_path ();

  /* NULL GError
   *
   * We don't check the return value of this.  If the file doesn't exist, we'll
   * simply want to create it.
   */
  g_key_file_load_from_file (key_file, key_file_path, 0, NULL);

  g_snprintf (str, sizeof (str), "%o", (guint) need_mask); /* octal, baby */
  g_key_file_set_string (key_file, path, "need_mask", str);

  save_key_file (key_file_path, key_file);

  g_key_file_free (key_file);
  g_free (key_file_path);
}

static void
remove_permissions (const char *path, mode_t need_mask)
{
  struct stat st;
  mode_t new_mode;

  if (need_mask == 0)
    return;

  if (stat (path, &st) != 0)
    return;

  new_mode = st.st_mode & ~need_mask;

  /* Bleah, no error checking */
  chmod (path, new_mode);
}

static void
remove_from_saved_permissions (const char *path, mode_t remove_mask)
{
  GKeyFile *key_file;
  char *key_file_path;

  if (remove_mask == 0)
    return;

  key_file = g_key_file_new ();
  key_file_path = get_key_file_path ();

  if (g_key_file_load_from_file (key_file, key_file_path, 0, NULL))
    {
      mode_t need_mask;
      mode_t remove_from_current_mask;
      char *str;

      need_mask = 0;

      /* NULL GError */
      str = g_key_file_get_string (key_file, path, "need_mask", NULL);

      if (str)
        {
          guint i;

          if (sscanf (str, "%o", &i) == 1) /* octal */
            need_mask = i;

          g_free (str);
        }

      remove_from_current_mask = need_mask & remove_mask;
      remove_permissions (path, remove_from_current_mask);

      need_mask &= ~remove_mask;

      if (need_mask == 0)
        {
          /* NULL GError */
          g_key_file_remove_group (key_file, path, NULL);
        }
      else
        {
          char buf[50];

          g_snprintf (buf, sizeof (buf), "%o", (guint) need_mask); /* octal */
          g_key_file_set_string (key_file, path, "need_mask", buf);
        }

      save_key_file (key_file_path, key_file);
    }

  g_key_file_free (key_file);
  g_free (key_file_path);
}

static void
restore_saved_permissions (const char *path)
{
  remove_from_saved_permissions (path, NEED_ALL_MASK);
}

static void
restore_write_permissions (const char *path)
{
  remove_from_saved_permissions (path, NEED_IF_WRITABLE_MASK);
}

typedef enum {
  CONFIRM_CANCEL_OR_ERROR,
  CONFIRM_NO_MODIFICATIONS,
  CONFIRM_MODIFIED
} ConfirmPermissionsStatus;

static ConfirmPermissionsStatus
confirm_sharing_permissions (GtkWidget *widget, const char *path, gboolean is_shared, gboolean guest_ok, gboolean is_writable)
{
  struct stat st;
  mode_t mode, new_mode, need_mask;

  if (!is_shared)
    return CONFIRM_NO_MODIFICATIONS;

  if (stat (path, &st) != 0)
    return CONFIRM_NO_MODIFICATIONS; /* We'll just let "net usershare" give back an error if the file disappears */

  new_mode = mode = st.st_mode;

  if (guest_ok)
    new_mode |= NEED_IF_GUESTOK_MASK;
  if (is_writable)
    new_mode |= NEED_IF_WRITABLE_MASK;

  need_mask = new_mode & ~mode;

  if (need_mask != 0)
    {
      g_assert (mode != new_mode);

      if (!message_confirm_missing_permissions (widget, path, need_mask))
        return CONFIRM_CANCEL_OR_ERROR;

      if (chmod (path, new_mode) != 0)
        {
          error_when_changing_permissions (widget, path);
          return CONFIRM_CANCEL_OR_ERROR;
        }

      save_changed_permissions (path, need_mask);

      return CONFIRM_MODIFIED;
    }
  else
    {
      g_assert (mode == new_mode);
      return CONFIRM_NO_MODIFICATIONS;
    }

  g_assert_not_reached ();
  return CONFIRM_CANCEL_OR_ERROR;
}

static gboolean
property_page_commit (PropertyPage *page)
{
  gboolean is_shared;
  ShareInfo share_info;
  ConfirmPermissionsStatus status;
  GError *error;
  gboolean retval;

  is_shared = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_folder));

  share_info.path = page->path;
  share_info.share_name = (char *) gtk_entry_get_text (GTK_ENTRY (page->entry_share_name));
  share_info.comment = (char *) gtk_entry_get_text (GTK_ENTRY (page->entry_share_comment));
  share_info.is_writable = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_rw_ro));
  share_info.guest_ok = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_guest_ok));

  /* Do we need to unset the write permissions that we added in the past? */
  if (is_shared && page->was_writable && !share_info.is_writable)
    restore_write_permissions (page->path);

  status = confirm_sharing_permissions (page->main, page->path, is_shared, share_info.guest_ok, share_info.is_writable);
  if (status == CONFIRM_CANCEL_OR_ERROR)
    return FALSE; /* the user didn't want us to change his folder's permissions */

  error = NULL;
  retval = shares_modify_share (share_info.path, is_shared ? &share_info : NULL, &error);

  if (!retval)
    {
      property_page_set_error (page, error->message);
      g_error_free (error);

      /* Since the operation failed, we restore things to the way they were */
      if (status == CONFIRM_MODIFIED)
        restore_saved_permissions (page->path);
    }
  else
    {
      property_page_validate_fields (page);
      caja_file_info_invalidate_extension_info (page->fileinfo);
    }

  if (!is_shared)
    restore_saved_permissions (page->path);

  /* update initially shared state, so that we may undo later on */
  if (retval)
    {
      page->was_initially_shared = is_shared;
      page->is_dirty = FALSE;
    }

  return retval;
}

/*--------------------------------------------------------------------------*/
static gchar *
get_fullpath_from_fileinfo(CajaFileInfo *fileinfo)
{
  GFile *file;
  gchar *fullpath;

  g_assert (fileinfo != NULL);
  
  file = caja_file_info_get_location(fileinfo);
  fullpath = g_file_get_path(file);
  g_assert (fullpath != NULL && g_file_is_native(file)); /* In the beginning we checked that this was a local URI */
  g_object_unref(file);

  return(fullpath);
}


/*--------------------------------------------------------------------------*/
static void
property_page_set_warning (PropertyPage *page)
{
  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (page->entry_share_name));
  if (gtk_style_context_has_class (context, GTK_STYLE_CLASS_ERROR))
    {
      gtk_style_context_remove_class (context, GTK_STYLE_CLASS_ERROR);
    }
  gtk_style_context_add_class (context, GTK_STYLE_CLASS_WARNING);

  gtk_label_set_text (GTK_LABEL (page->label_status), _("Share name is too long"));
}

static void
property_page_set_error (PropertyPage *page, const char *message)
{
  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (page->entry_share_name));
  if (gtk_style_context_has_class (context, GTK_STYLE_CLASS_WARNING))
    {
      gtk_style_context_remove_class (context, GTK_STYLE_CLASS_WARNING);
    }
  gtk_style_context_add_class (context, GTK_STYLE_CLASS_ERROR);

  gtk_label_set_text (GTK_LABEL (page->label_status), message);
}

static void
property_page_set_normal (PropertyPage *page)
{
  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (page->entry_share_name));
  if (gtk_style_context_has_class (context, GTK_STYLE_CLASS_WARNING))
    {
      gtk_style_context_remove_class (context, GTK_STYLE_CLASS_WARNING);
    }

  if (gtk_style_context_has_class (context, GTK_STYLE_CLASS_ERROR))
    {
      gtk_style_context_remove_class (context, GTK_STYLE_CLASS_ERROR);
    }

  gtk_label_set_text (GTK_LABEL (page->label_status), "");
}

static gboolean
property_page_share_name_is_valid (PropertyPage *page)
{
  const char *newname;

  newname = gtk_entry_get_text (GTK_ENTRY (page->entry_share_name));

  if (strlen (newname) == 0)
    {
      property_page_set_error (page, _("The share name cannot be empty"));
      return FALSE;
    }
  else
    {
      GError *error;
      gboolean exists;

      error = NULL;
      if (!shares_get_share_name_exists (newname, &exists, &error))
        {
          char *str;

          str = g_strdup_printf (_("Error while getting share information: %s"), error->message);
          property_page_set_error (page, str);
          g_free (str);
          g_error_free (error);

          return FALSE;
        }

      if (exists)
        {
          property_page_set_error (page, _("Another share has the same name"));
          return FALSE;
        }
      else
        {
          property_page_set_normal (page);
          return TRUE;
        }
    }
}

static void
property_page_set_controls_sensitivity (PropertyPage *page,
					gboolean      sensitive)
{
  gtk_widget_set_sensitive (page->entry_share_name, sensitive);
  gtk_widget_set_sensitive (page->entry_share_comment, sensitive);
  gtk_widget_set_sensitive (page->hbox_share_comment, sensitive);
  gtk_widget_set_sensitive (page->hbox_share_name, sensitive);
  gtk_widget_set_sensitive (page->checkbutton_share_rw_ro, sensitive);

  if (sensitive)
    {
      gboolean guest_ok_allowed;
      shares_supports_guest_ok (&guest_ok_allowed, NULL);
      gtk_widget_set_sensitive (page->checkbutton_share_guest_ok, guest_ok_allowed);
    }
  else
    gtk_widget_set_sensitive (page->checkbutton_share_guest_ok, FALSE);
}

static void
property_page_check_sensitivity (PropertyPage *page)
{
  gboolean enabled;
  gboolean apply_is_sensitive;

  enabled = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_folder));
  property_page_set_controls_sensitivity (page, enabled);

  if (enabled)
    apply_is_sensitive = page->is_dirty || !page->was_initially_shared;
  else
    apply_is_sensitive = page->was_initially_shared;

  gtk_widget_set_sensitive (page->button_apply, apply_is_sensitive);
  gtk_button_set_label (GTK_BUTTON(page->button_apply),
			page->was_initially_shared ? _("Modify _Share") : _("Create _Share"));
}

static void
modify_share_name_text_entry  (GtkEditable *editable,
			       gpointer user_data)
{
  PropertyPage *page;

  page = user_data;

  page->is_dirty = TRUE;

  /* This function does simple validation on the share name and sets the error
   * label; just let it run and ignore the result value.
   */
  property_page_share_name_is_valid (page);

  property_page_check_sensitivity (page);
}

static void
modify_share_comment_text_entry  (GtkEditable *editable,
				  gpointer user_data)
{
  PropertyPage *page;

  page = user_data;

  page->is_dirty = TRUE;
  property_page_check_sensitivity (page);
}

/*--------------------------------------------------------------------------*/
static void
on_checkbutton_share_folder_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  PropertyPage *page;

  page = user_data;

  property_page_check_sensitivity (page);
}

static void
on_checkbutton_rw_ro_toggled    (GtkToggleButton *togglebutton,
				 gpointer         user_data)
{
  PropertyPage *page;

  page = user_data;

  page->is_dirty = TRUE;

  property_page_check_sensitivity (page);
}

static void
on_checkbutton_guest_ok_toggled    (GtkToggleButton *togglebutton,
				    gpointer         user_data)
{
  PropertyPage *page;

  page = user_data;

  page->is_dirty = TRUE;

  property_page_check_sensitivity (page);
}

static void
free_property_page_cb (gpointer data)
{
  PropertyPage *page;

  page = data;

  g_free (page->path);
  g_object_unref (page->fileinfo);
  g_object_unref (page->ui);

  g_free (page);
}

static void
button_apply_clicked_cb (GtkButton *button,
			 gpointer   data)
{
  PropertyPage *page;

  page = data;

  if (property_page_commit (page))
    {
      if (page->standalone_window)
	gtk_widget_destroy (page->standalone_window);
      else
        property_page_check_sensitivity (page);
    }
}

/*--------------------------------------------------------------------------*/
static PropertyPage *
create_property_page (CajaFileInfo *fileinfo)
{
  PropertyPage *page;
  GError *error;
  ShareInfo *share_info;
  char *share_name;
  gboolean free_share_name;
  const char *comment;
  char *apply_button_label;

  page = g_new0 (PropertyPage, 1);

  page->path = get_fullpath_from_fileinfo(fileinfo);
  page->fileinfo = g_object_ref (fileinfo);

  error = NULL;
  if (!shares_get_share_info_for_path (page->path, &share_info, &error))
    {
      /* We'll assume that there is no share for that path, but we'll still
       * bring up an error dialog.
       */
      GtkWidget *message;

      message = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
					_("There was an error while getting the sharing information"));
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s", error->message);
      gtk_widget_show (message);

      share_info = NULL;
      g_error_free (error);
      error = NULL;
    }


  page->ui = gtk_builder_new ();
  gtk_builder_set_translation_domain (page->ui, GETTEXT_PACKAGE);
  g_assert (gtk_builder_add_from_file (page->ui,
              INTERFACES_DIR"/share-dialog.ui", &error));

  page->main = GTK_WIDGET (gtk_builder_get_object (page->ui, "vbox1"));
  g_assert (page->main != NULL);

  g_object_set_data_full (G_OBJECT (page->main),
			  "PropertyPage",
			  page,
			  free_property_page_cb);

  page->checkbutton_share_folder = GTK_WIDGET (gtk_builder_get_object (page->ui,"checkbutton_share_folder"));
  page->hbox_share_comment = GTK_WIDGET (gtk_builder_get_object (page->ui,"hbox_share_comment"));
  page->hbox_share_name = GTK_WIDGET (gtk_builder_get_object (page->ui,"hbox_share_name"));
  page->checkbutton_share_rw_ro = GTK_WIDGET (gtk_builder_get_object (page->ui,"checkbutton_share_rw_ro"));
  page->checkbutton_share_guest_ok = GTK_WIDGET (gtk_builder_get_object (page->ui,"checkbutton_share_guest_ok"));
  page->entry_share_name = GTK_WIDGET (gtk_builder_get_object (page->ui,"entry_share_name"));
  page->entry_share_comment = GTK_WIDGET (gtk_builder_get_object (page->ui,"entry_share_comment"));
  page->label_status = GTK_WIDGET (gtk_builder_get_object (page->ui,"label_status"));
  page->button_cancel = GTK_WIDGET (gtk_builder_get_object (page->ui,"button_cancel"));
  page->button_apply = GTK_WIDGET (gtk_builder_get_object (page->ui,"button_apply"));

  /* Sanity check so that we don't screw up the Glade file */
  g_assert (page->checkbutton_share_folder != NULL
	    && page->hbox_share_comment != NULL
	    && page->hbox_share_name != NULL
	    && page->checkbutton_share_rw_ro != NULL
	    && page->checkbutton_share_guest_ok != NULL
	    && page->entry_share_name != NULL
	    && page->entry_share_comment != NULL
	    && page->label_status != NULL
	    && page->button_cancel != NULL
	    && page->button_apply != NULL);

  if (share_info)
    {
      page->was_initially_shared = TRUE;
      page->was_writable = share_info->is_writable;
    }

  /* Share name */

  if (share_info)
    {
      share_name = share_info->share_name;
      free_share_name = FALSE;
    }
  else
    {
      share_name = g_filename_display_basename (page->path);
      free_share_name = TRUE;
    }

  gtk_entry_set_text (GTK_ENTRY (page->entry_share_name), share_name);

  if (free_share_name)
    g_free (share_name);

  /* Comment */

  if (share_info == NULL || share_info->comment == NULL)
    comment = "";
  else
    comment = share_info->comment;

  gtk_entry_set_text (GTK_ENTRY (page->entry_share_comment), comment);

  /* Share toggle */

  if (share_info)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_folder), TRUE);
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_folder), FALSE);
    }

  /* Share name */

  if (g_utf8_strlen(gtk_entry_get_text (GTK_ENTRY (page->entry_share_name)), -1) > 12)
    property_page_set_warning (page);

  /* Permissions */
  if (share_info != NULL && share_info->is_writable)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_rw_ro), TRUE);
  else
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_rw_ro), FALSE);

  /* Guest access */
  if (share_info != NULL && share_info->guest_ok)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_guest_ok), TRUE);
  else
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->checkbutton_share_guest_ok), FALSE);

  /* Apply button */

  if (share_info)
    apply_button_label = _("Modify _Share");
  else
    apply_button_label = _("Create _Share");

  gtk_button_set_label (GTK_BUTTON (page->button_apply), apply_button_label);
  gtk_button_set_use_underline (GTK_BUTTON (page->button_apply), TRUE);
  gtk_button_set_image (GTK_BUTTON (page->button_apply), gtk_image_new_from_icon_name ("document-save", GTK_ICON_SIZE_BUTTON));

  gtk_widget_set_sensitive (page->button_apply, FALSE);

  /* Sensitivity */

  property_page_check_sensitivity (page);

  /* Signal handlers */

  g_signal_connect (page->checkbutton_share_folder, "toggled",
                    G_CALLBACK (on_checkbutton_share_folder_toggled),
                    page);

  g_signal_connect (page->checkbutton_share_rw_ro, "toggled",
                    G_CALLBACK (on_checkbutton_rw_ro_toggled),
                    page);

  g_signal_connect (page->checkbutton_share_guest_ok, "toggled",
                    G_CALLBACK (on_checkbutton_guest_ok_toggled),
                    page);

  g_signal_connect (page->entry_share_name, "changed",
                    G_CALLBACK (modify_share_name_text_entry),
                    page);

  g_signal_connect (page->entry_share_comment, "changed",
		    G_CALLBACK (modify_share_comment_text_entry),
		    page);

  g_signal_connect (page->button_apply, "clicked",
		    G_CALLBACK (button_apply_clicked_cb), page);

  if (share_info != NULL)
    shares_free_share_info (share_info);

  return page;
}

/* Implementation of the CajaInfoProvider interface */

/* caja_info_provider_update_file_info 
 * This function is called by Caja when it wants the extension to 
 * fill in data about the file.  It passes a CajaFileInfo object,
 * which the extension can use to read data from the file, and which
 * the extension should add data to.
 *
 * If the data can be added immediately (without doing blocking IO), 
 * the extension can do so, and return CAJA_OPERATION_COMPLETE.  
 * In this case the 'update_complete' and 'handle' parameters can be 
 * ignored.
 * 
 * If waiting for the deata would block the UI, the extension should
 * perform the task asynchronously, and return 
 * CAJA_OPERATION_IN_PROGRESS.  The function must also set the 
 * 'handle' pointer to a value unique to the object, and invoke the
 * 'update_complete' closure when the update is done.
 * 
 * If the extension encounters an error, it should return 
 * CAJA_OPERATION_FAILED.
 */
typedef struct {
  gboolean cancelled;
  CajaInfoProvider *provider;
  CajaFileInfo *file;
  GClosure *update_complete;
} CajaShareHandle;

static CajaShareStatus 
get_share_status_and_free_share_info (ShareInfo *share_info)
{
  CajaShareStatus result;

  if (!share_info)
    result = CAJA_SHARE_NOT_SHARED;
  else
    {
      if (share_info->is_writable)
	result = CAJA_SHARE_SHARED_RW;
      else
	result = CAJA_SHARE_SHARED_RO;

      shares_free_share_info (share_info);
    }

  return result;
}


/*--------------------------------------------------------------------------*/
static void
get_share_info_for_file_info (CajaFileInfo *file, ShareInfo **share_info, gboolean *is_shareable)
{
  char		*uri;
  char		*local_path = NULL;
  GFile         *f;

  *share_info = NULL;
  *is_shareable = FALSE;

  uri = caja_file_info_get_uri (file);
  f = caja_file_info_get_location(file);
  if (!uri)
    goto out;

#define NETWORK_SHARE_PREFIX "network:///share-"

  if (g_str_has_prefix (uri, NETWORK_SHARE_PREFIX))
    {
      const char *share_name;

      share_name = uri + strlen (NETWORK_SHARE_PREFIX);

      /* FIXME: NULL GError */
      if (!shares_get_share_info_for_share_name (share_name, share_info, NULL))
        {
          *share_info = NULL;
          *is_shareable = TRUE; /* it *has* the prefix, anyway... we are just unsynchronized with what mate-vfs thinks */
        }
      else
        {
          *is_shareable = TRUE;
        }

      goto out;
    }

  if (!caja_file_info_is_directory(file))
    goto out;

  local_path = g_file_get_path(f);
  if (!local_path || !g_file_is_native(f))
    goto out;

  /* FIXME: NULL GError */
  if (!shares_get_share_info_for_path (local_path, share_info, NULL))
    goto out;

  *is_shareable = TRUE;

 out:

  g_object_unref(f);
  g_free (uri);
  g_free (local_path);
}

/*--------------------------------------------------------------------------*/
static CajaShareStatus 
file_get_share_status_file(CajaFileInfo *file)
{
  ShareInfo *share_info;
  gboolean is_shareable;

  get_share_info_for_file_info (file, &share_info, &is_shareable);

  if (!is_shareable)
    return CAJA_SHARE_NOT_SHARED;

  return get_share_status_and_free_share_info (share_info);
}

static CajaOperationResult
caja_share_update_file_info (CajaInfoProvider *provider,
				 CajaFileInfo *file,
				 GClosure *update_complete,
				 CajaOperationHandle **handle)
{
/*   gchar *share_status = NULL; */
  
  switch (file_get_share_status_file (file)) {

  case CAJA_SHARE_SHARED_RO:
    caja_file_info_add_emblem (file, "shared");
/*     share_status = _("shared (read only)"); */
    break;

  case CAJA_SHARE_SHARED_RW:
    caja_file_info_add_emblem (file, "shared");
/*     share_status = _("shared (read and write)"); */
    break;

  case CAJA_SHARE_NOT_SHARED:
/*     share_status = _("not shared"); */
    break;

  default:
    g_assert_not_reached ();
    break;
  }

/*   caja_file_info_add_string_attribute (file, */
/* 					   "CajaShare::share_status", */
/* 					   share_status); */
  return CAJA_OPERATION_COMPLETE;
}


static void
caja_share_cancel_update (CajaInfoProvider *provider,
			      CajaOperationHandle *handle)
{
  CajaShareHandle *share_handle;
	
  share_handle = (CajaShareHandle*)handle;
  share_handle->cancelled = TRUE;
}

static void 
caja_share_info_provider_iface_init (CajaInfoProviderIface *iface)
{
  iface->update_file_info = caja_share_update_file_info;
  iface->cancel_update = caja_share_cancel_update;
}

/*--------------------------------------------------------------------------*/
/* caja_property_page_provider_get_pages
 *  
 * This function is called by Caja when it wants property page
 * items from the extension.
 *
 * This function is called in the main thread before a property page
 * is shown, so it should return quickly.
 * 
 * The function should return a GList of allocated CajaPropertyPage
 * items.
 */
static GList *
caja_share_get_property_pages (CajaPropertyPageProvider *provider,
				   GList *files)
{
  PropertyPage *page;
  GList *pages;
  CajaPropertyPage *np_page;
  CajaFileInfo *fileinfo;
  ShareInfo *share_info;
  gboolean is_shareable;

  /* Only show the property page if 1 file is selected */
  if (!files || files->next != NULL) {
    return NULL;
  }

  fileinfo = CAJA_FILE_INFO (files->data);

  get_share_info_for_file_info (fileinfo, &share_info, &is_shareable);
  if (!is_shareable)
    return NULL;

  page = create_property_page (fileinfo);
  gtk_widget_hide (page->button_cancel);
  
  if (share_info)
    shares_free_share_info (share_info);

  pages = NULL;
  np_page = caja_property_page_new
    ("CajaShare::property_page",
     gtk_label_new (_("Share")),
     page->main);
  pages = g_list_append (pages, np_page);

  return pages;
}

/*--------------------------------------------------------------------------*/
static void 
caja_share_property_page_provider_iface_init (CajaPropertyPageProviderIface *iface)
{
  iface->get_pages = caja_share_get_property_pages;
}

/*--------------------------------------------------------------------------*/
static void
caja_share_instance_init (CajaShare *share)
{
}

/*--------------------------------------------------------------------------*/
static void
caja_share_class_init (CajaShareClass *class)
{
  parent_class = g_type_class_peek_parent (class);
}

/* caja_menu_provider_get_file_items
 *  
 * This function is called by Caja when it wants context menu
 * items from the extension.
 *
 * This function is called in the main thread before a context menu
 * is shown, so it should return quickly.
 * 
 * The function should return a GList of allocated CajaMenuItem
 * items.
 */

static void
button_cancel_clicked_cb (GtkButton *button, gpointer data)
{
  GtkWidget *window;

  window = GTK_WIDGET (data);
  gtk_widget_destroy (window);
}

static void
share_this_folder_callback (CajaMenuItem *item,
			    gpointer user_data)
{
  CajaFileInfo *fileinfo;
  PropertyPage *page;
  GtkWidget * window;

  fileinfo = CAJA_FILE_INFO (user_data);
  g_assert (fileinfo != NULL);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), _("Folder Sharing"));
  page = create_property_page (fileinfo);
  page->standalone_window = window;
  g_signal_connect (page->button_cancel, "clicked",
		    G_CALLBACK (button_cancel_clicked_cb), window);

  gtk_container_add (GTK_CONTAINER (window), page->main);
  gtk_widget_show (window);
}

static GList *
caja_share_get_file_items (CajaMenuProvider *provider,
			     GtkWidget *window,
			     GList *files)
{
  GList *items;
  CajaMenuItem *item;
  CajaFileInfo *fileinfo;
  ShareInfo *share_info;
  gboolean is_shareable;

  /* Only show the property page if 1 file is selected */
  if (!files || files->next != NULL) {
    return NULL;
  }

  fileinfo = CAJA_FILE_INFO (files->data);

  get_share_info_for_file_info (fileinfo, &share_info, &is_shareable);

  if (!is_shareable)
    return NULL;

  if (share_info)
    shares_free_share_info (share_info);

  /* We don't own a reference to the file info to keep it around, so acquire one */
  g_object_ref (fileinfo);
  
  /* FMQ: change the label to "Share with Windows users"? */
  item = caja_menu_item_new ("CajaShare::share",
				 _("Sharing Options"),
				 _("Share this Folder"),
				 "folder-remote");
  g_signal_connect (item, "activate",
		    G_CALLBACK (share_this_folder_callback),
		    fileinfo);
  g_object_set_data_full (G_OBJECT (item), 
			  "files",
			  fileinfo,
			  g_object_unref); /* Release our reference when the menu item goes away */

  items = g_list_append (NULL, item);
  return items;
}

/*--------------------------------------------------------------------------*/
static void 
caja_share_menu_provider_iface_init (CajaMenuProviderIface *iface)
{
	iface->get_file_items = caja_share_get_file_items;
}

/*--------------------------------------------------------------------------*/
/* Type registration.  Because this type is implemented in a module
 * that can be unloaded, we separate type registration from get_type().
 * the type_register() function will be called by the module's
 * initialization function. */
static GType share_type = 0;

#define CAJA_TYPE_SHARE  (caja_share_get_type ())

static GType
caja_share_get_type (void) 
{
  return share_type;
}

static void
caja_share_register_type (GTypeModule *module)
{
  static const GTypeInfo info = {
    sizeof (CajaShareClass),
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) caja_share_class_init,
    NULL, 
    NULL,
    sizeof (CajaShare),
    0,
    (GInstanceInitFunc) caja_share_instance_init,
  };

  share_type = g_type_module_register_type (module,
					    G_TYPE_OBJECT,
					    "CajaShare",
					    &info, 0);

  static const GInterfaceInfo property_page_provider_iface_info = {
    (GInterfaceInitFunc) caja_share_property_page_provider_iface_init,
    NULL,
    NULL
  };
	
  g_type_module_add_interface (module,
			       share_type,
			       CAJA_TYPE_PROPERTY_PAGE_PROVIDER,
			       &property_page_provider_iface_info);


  static const GInterfaceInfo info_provider_iface_info = {
    (GInterfaceInitFunc) caja_share_info_provider_iface_init,
    NULL,
    NULL
  };

  g_type_module_add_interface (module,
			       share_type,
			       CAJA_TYPE_INFO_PROVIDER,
			       &info_provider_iface_info);

  /* Menu right clik */
  static const GInterfaceInfo menu_provider_iface_info = {
    (GInterfaceInitFunc) caja_share_menu_provider_iface_init,
    NULL,
    NULL
  };
  
  g_type_module_add_interface (module,
			       share_type,
			       CAJA_TYPE_MENU_PROVIDER,
			       &menu_provider_iface_info);
  
}

/* Extension module functions.  These functions are defined in 
 * caja-extensions-types.h, and must be implemented by all 
 * extensions. */

/* Initialization function.  In addition to any module-specific 
 * initialization, any types implemented by the module should 
 * be registered here. */
void
caja_module_initialize (GTypeModule  *module)
{
  /*g_print ("Initializing caja-share extension\n");*/

  bindtextdomain(GETTEXT_PACKAGE, MATELOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

  caja_share_register_type (module);
}

/* Perform module-specific shutdown. */
void
caja_module_shutdown   (void)
{
  /*g_print ("Shutting down caja-share extension\n");*/
  /* FIXME freeing */
}

/* List all the extension types.  */
void 
caja_module_list_types (const GType **types,
			    int          *num_types)
{
  static GType type_list[1];
	
  type_list[0] = CAJA_TYPE_SHARE;

  *types = type_list;
  *num_types = 1;
}
