plugindir = $(libdir)/caja-sendto/plugins

AM_CPPFLAGS =                                      \
        -DDATADIR=\"$(datadir)\"                \
        -DICONDIR=\"$(icondir)\"                \
	-DLOCALEDIR="\"$(datadir)/locale\""	\
        -I$(top_srcdir)/sendto                  \
        -I$(top_builddir)                       \
        -I$(srcdir)/../				\
        $(SENDTO_CFLAGS)               \
	$(WARN_CFLAGS)				\
	$(DISABLE_DEPRECATED)                   \
	$(GIO_CFLAGS)

plugin_LTLIBRARIES = libnstburn.la

libnstburn_la_SOURCES = caja-burn.c
libnstburn_la_LDFLAGS = -module -avoid-version
libnstburn_la_LIBADD = $(GIO_LIBS) $(SENDTO_LIBS) $(builddir)/../libnstcommon.la

