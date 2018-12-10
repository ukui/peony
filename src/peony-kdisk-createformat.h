#ifndef kDISK_CREATEFORMAT_H
#define KDISK_CREATEFORMAT_H

#include <sys/stat.h>
#include <udisks/udisks.h>
#include <glib/gi18n.h>
#include <errno.h>

void    kdisk_format(const gchar * device_name,const gchar * format_type,const gchar * is_erase,const gchar * filesystem_name,int *format_finish);
double  get_device_size(const gchar * device_name);
void cancel_format(const gchar* device_name);


int find_cb(const gchar *device_name);

double get_format_bytes_done(const gchar *device_name);

gchar * get_device_label(const gchar * device_name);
#endif // CREATEFORMAT_H#include <stdio.h>
