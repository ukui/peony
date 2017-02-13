

#ifndef __PEONY_RECENT_H__
#define __PEONY_RECENT_H__

#include <gtk/gtk.h>
#include <libpeony-private/peony-file.h>
#include <gio/gio.h>

void peony_recent_add_file (PeonyFile *file,
                           GAppInfo *application);

#endif
