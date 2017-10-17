#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-menu-provider.h>

#include "../config.h"

#include <libintl.h>
#define _(x) dgettext (GETTEXT_PACKAGE, x)

#define GKSU_TYPE_CONTEXT_MENU (gksu_context_menu_get_type ())
#define GKSU_CONTEXT_MENU(o)   (G_TYPE_CHECK_INSTANCE_CAST ((o), GKSU_TYPE_CONTEXT_MENU))

typedef struct {
    GObject parent;
} GksuContextMenu;

typedef struct {
    GObjectClass parent_class;
} GksuContextMenuClass;

static GType gksucm_type = 0;
static GObjectClass *parent_class = NULL;

static void
gksu_context_menu_init (GksuContextMenu *self);
static void
gksu_context_menu_class_init (GksuContextMenuClass *class);
static void
menu_provider_iface_init (CajaMenuProviderIface *iface);

static GList*
gksu_context_menu_get_file_items (CajaMenuProvider *provider,
				  GtkWidget *window,
				  GList *files);
static void
gksu_context_menu_activate (CajaMenuItem *item,
			    CajaFileInfo *file);

static GType
gksu_context_menu_get_type (void)
{
    return gksucm_type;
}

static void
gksu_context_menu_register_type (GTypeModule *module)
{
    static const GTypeInfo info = {
	sizeof (GksuContextMenuClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gksu_context_menu_class_init,
	NULL,
	NULL,
	sizeof (GksuContextMenu),
	0,
	(GInstanceInitFunc) gksu_context_menu_init
    };
    static const GInterfaceInfo menu_provider_iface_info = {
	(GInterfaceInitFunc)menu_provider_iface_init,
	NULL,
	NULL
    };

    gksucm_type = g_type_module_register_type (module,
					    G_TYPE_OBJECT,
					    "GksuContextMenu",
					    &info, 0);
    g_type_module_add_interface (module,
				 gksucm_type,
				 CAJA_TYPE_MENU_PROVIDER,
				 &menu_provider_iface_info);
}

static void
gksu_context_menu_class_init (GksuContextMenuClass *class)
{
    parent_class = g_type_class_peek_parent (class);
}

static void menu_provider_iface_init (CajaMenuProviderIface *iface)
{
    iface->get_file_items = gksu_context_menu_get_file_items;
}

static void
gksu_context_menu_init (GksuContextMenu *self)
{
  g_message ("Initializing gksu extension...");
}

static GList *
gksu_context_menu_get_file_items (CajaMenuProvider *provider,
				  GtkWidget *window,
				  GList *files)
{
    GList *items = NULL;
    CajaFileInfo *file;
    CajaMenuItem *item;

    /* if we're already root, really or effectively, do not add
       the menu item */
    if (geteuid () == 0)
      return NULL;

    /* only add a menu item if a single file is selected ... */
    if (files == NULL || files->next != NULL)
      return NULL;

    file = files->data;

    /* ... and if it is not a caja special item */
    {
      gchar *uri_scheme = NULL;

      uri_scheme = caja_file_info_get_uri_scheme (file);
      if (!strncmp (uri_scheme, "x-caja-desktop", 18))
	{
	  g_free (uri_scheme);
	  return NULL;
	}
      g_free (uri_scheme);
    }

    /* create the context menu item */
    item = caja_menu_item_new ("Gksu::open_as_root",
				   _("Open as administrator"),
				   _("Opens the file with administrator privileges"),
				   NULL);
    g_signal_connect_object (item, "activate",
			     G_CALLBACK (gksu_context_menu_activate),
			     file, 0);
    items = g_list_prepend (items, item);

    return items;
}

gboolean
is_gksu_dead (gpointer data)
{
  GPid pid = GPOINTER_TO_INT(data);
  if (waitpid (pid, NULL, WNOHANG) > 0)
    return FALSE;
  return TRUE;
}

static void*
start_gksu_thread (void *data)
{
  GPid pid;
  gchar **argv = (gchar**) g_malloc (sizeof (gchar*) * 3);
  gchar *full_cmd = (gchar*) data;

  argv[0] = g_strdup ("gksu");
  argv[1] = full_cmd;
  argv[2] = NULL;

  g_spawn_async (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL,
		 &pid, NULL);
  g_timeout_add (5000, is_gksu_dead, GINT_TO_POINTER(pid));

  g_free (argv[0]);
  g_free (full_cmd);
  g_free (argv);

  return NULL;
}

static void
gksu_context_menu_activate (CajaMenuItem *item,
			    CajaFileInfo *file)
{
  gchar *uri = NULL;
  gchar *mime_type = NULL;
  gchar *cmd = NULL;
  gchar *full_cmd = NULL;
  gchar *tmp = NULL;
  gboolean is_desktop = FALSE;

  uri = caja_file_info_get_uri (file);
  mime_type = caja_file_info_get_mime_type (file);

  if (!strcmp (mime_type, "application/x-desktop"))
    { /* we're handling a .desktop file */
      GKeyFile *key_file = g_key_file_new ();
      gint retval = 0;

      is_desktop = TRUE;

      gchar *file_path = g_filename_from_uri (uri, NULL, NULL);
      retval = g_key_file_load_from_file (key_file, file_path, 0, NULL);
      g_free (file_path);

      if (retval)
        cmd = g_key_file_get_string (key_file, "Desktop Entry", "Exec", NULL);
      g_key_file_free (key_file);
    }
  else
    {
      GAppInfo *app_info = g_app_info_get_default_for_type (mime_type, strncmp (uri, "file://", 7));
      if (app_info)
	{
	  cmd = g_strdup (g_app_info_get_executable (app_info));
          g_object_unref (app_info);
	}
    }

  if (cmd == NULL)
    {
      GtkWidget *dialog;

      dialog = gtk_message_dialog_new_with_markup (NULL, 0,
						   GTK_MESSAGE_ERROR,
						   GTK_BUTTONS_CLOSE,
						   _("<big><b>"
						     "Unable to determine the program to run."
						     "</b></big>\n\n"
						     "The item you selected cannot be open with "
						     "administrator powers because the correct "
						     "application cannot be determined."));
      gtk_dialog_run (GTK_DIALOG(dialog));
      gtk_widget_destroy (dialog);
      return;
    }

  /*
   * FIXME: remove any FreeDesktop substitution variable for now; we
   * need to process them!
   */
  tmp = strstr (cmd, "%");
  if (tmp)
    *tmp = '\0';

  if (is_desktop)
    full_cmd = cmd;
  else
    {
      full_cmd = g_strdup_printf ("%s '%s'", cmd, uri);
      g_free (cmd);
    }

  {
    pthread_t new_thread;
    pthread_create (&new_thread, NULL, start_gksu_thread, (void*)full_cmd);
  }

  /* full_cmd is freed by start_gksu_thread */
  g_free (uri);
  g_free (mime_type);
}

/* --- extension interface --- */
void
caja_module_initialize (GTypeModule *module)
{
    gksu_context_menu_register_type (module);
}

void
caja_module_shutdown (void)
{
}

void
caja_module_list_types (const GType **types,
			    int *num_types)
{
    static GType type_list[1];

    type_list[0] = GKSU_TYPE_CONTEXT_MENU;
    *types = type_list;
    *num_types = G_N_ELEMENTS (type_list);
}
