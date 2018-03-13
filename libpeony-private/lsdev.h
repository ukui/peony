#include <stdio.h>
#include <glib.h>

#ifdef O_CLOEXEC
#define UL_CLOEXECSTR   "e"
#else
#define UL_CLOEXECSTR   ""
#endif


#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif

goffset process_one_device(char *devname);

