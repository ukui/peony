#ifndef PEONY_ICON_INFO_H
#define PEONY_ICON_INFO_H

#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdk.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* Names for Peony's different zoom levels, from tiniest items to largest items */
    typedef enum {
        PEONY_ZOOM_LEVEL_SMALLEST,
        PEONY_ZOOM_LEVEL_SMALLER,
        PEONY_ZOOM_LEVEL_SMALL,
        PEONY_ZOOM_LEVEL_STANDARD,
        PEONY_ZOOM_LEVEL_LARGE,
        PEONY_ZOOM_LEVEL_LARGER,
        PEONY_ZOOM_LEVEL_LARGEST
    }
    PeonyZoomLevel;

#define PEONY_ZOOM_LEVEL_N_ENTRIES (PEONY_ZOOM_LEVEL_LARGEST + 1)

    /* Nominal icon sizes for each Peony zoom level.
     * This scheme assumes that icons are designed to
     * fit in a square space, though each image needn't
     * be square. Since individual icons can be stretched,
     * each icon is not constrained to this nominal size.
     */
#define PEONY_ICON_SIZE_SMALLEST	16
#define PEONY_ICON_SIZE_SMALLER	24
#define PEONY_ICON_SIZE_SMALL	32
#define PEONY_ICON_SIZE_STANDARD	48
#define PEONY_ICON_SIZE_LARGE	72
#define PEONY_ICON_SIZE_LARGER	96
#define PEONY_ICON_SIZE_LARGEST     192

#define PEONY_DESKTOP_ICON_NUM_WIDTH_SMALL		20
#define PEONY_DESKTOP_ICON_NUM_WIDTH_STANDARD	13
#define PEONY_DESKTOP_ICON_NUM_WIDTH_LARGE		9
#define PEONY_DESKTOP_ICON_NUM_WIDTH_LARGER		7
#define PEONY_DESKTOP_ICON_NUM_WIDTH_LARGEST    3

#define PEONY_DESKTOP_ICON_NUM_HEIGHT_SMALL		10
#define PEONY_DESKTOP_ICON_NUM_HEIGHT_STANDARD	7
#define PEONY_DESKTOP_ICON_NUM_HEIGHT_LARGE		5
#define PEONY_DESKTOP_ICON_NUM_HEIGHT_LARGER	3
#define PEONY_DESKTOP_ICON_NUM_HEIGHT_LARGEST   2

    /* Maximum size of an icon that the icon factory will ever produce */
#define PEONY_ICON_MAXIMUM_SIZE     320

    typedef struct _PeonyIconInfo      PeonyIconInfo;
    typedef struct _PeonyIconInfoClass PeonyIconInfoClass;


#define PEONY_TYPE_ICON_INFO                 (peony_icon_info_get_type ())
#define PEONY_ICON_INFO(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_ICON_INFO, PeonyIconInfo))
#define PEONY_ICON_INFO_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_ICON_INFO, PeonyIconInfoClass))
#define PEONY_IS_ICON_INFO(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_ICON_INFO))
#define PEONY_IS_ICON_INFO_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_ICON_INFO))
#define PEONY_ICON_INFO_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_ICON_INFO, PeonyIconInfoClass))


    GType    peony_icon_info_get_type (void) G_GNUC_CONST;

    PeonyIconInfo *    peony_icon_info_new_for_pixbuf               (GdkPixbuf         *pixbuf);
    PeonyIconInfo *    peony_icon_info_lookup                       (GIcon             *icon,
            int                size);
    PeonyIconInfo *    peony_icon_info_lookup_from_name             (const char        *name,
            int                size);
    PeonyIconInfo *    peony_icon_info_lookup_from_path             (const char        *path,
            int                size);
    gboolean              peony_icon_info_is_fallback                  (PeonyIconInfo  *icon);
    GdkPixbuf *           peony_icon_info_get_pixbuf                   (PeonyIconInfo  *icon);
    GdkPixbuf *           peony_icon_info_get_pixbuf_nodefault         (PeonyIconInfo  *icon);
    GdkPixbuf *           peony_icon_info_get_pixbuf_nodefault_at_size (PeonyIconInfo  *icon,
            gsize              forced_size);
    GdkPixbuf *           peony_icon_info_get_pixbuf_at_size           (PeonyIconInfo  *icon,
            gsize              forced_size);
    gboolean              peony_icon_info_get_embedded_rect            (PeonyIconInfo  *icon,
            GdkRectangle      *rectangle);
    gboolean              peony_icon_info_get_attach_points            (PeonyIconInfo  *icon,
            GdkPoint         **points,
            gint              *n_points);
    const char* peony_icon_info_get_display_name(PeonyIconInfo* icon);
    const char* peony_icon_info_get_used_name(PeonyIconInfo* icon);

    void                  peony_icon_info_clear_caches                 (void);

    /* Relationship between zoom levels and icons sizes. */
    guint peony_get_icon_size_for_zoom_level          (PeonyZoomLevel  zoom_level);
    float peony_get_relative_icon_size_for_zoom_level (PeonyZoomLevel  zoom_level);

    guint peony_icon_get_larger_icon_size             (guint              size);
    guint peony_icon_get_smaller_icon_size            (guint              size);

    gint  peony_get_icon_size_for_stock_size          (GtkIconSize        size);
    guint peony_icon_get_emblem_size_for_icon_size    (guint              size);

gboolean peony_icon_theme_can_render              (GThemedIcon *icon);
GIcon * peony_user_special_directory_get_gicon (GUserDirectory directory);

#ifdef __cplusplus
}
#endif

#endif /* PEONY_ICON_INFO_H */

