plugindir = $(libdir)/caja-sendto/plugins

AM_CPPFLAGS =                                      \
        -DDATADIR=\"$(datadir)\"                \
        -DICONDIR=\"$(icondir)\"                \
        -DLOCALEDIR="\"$(datadir)/locale\""     \
        -I$(top_srcdir)/sendto                  \
        -I$(top_builddir)                       \
        $(SENDTO_CFLAGS)                   \
	$(EMAILCLIENT_CFLAGS)                   \
        $(DISABLE_DEPRECATED)                   \
        $(WARN_CFLAGS)

plugin_LTLIBRARIES = libnstemailclient.la

libnstemailclient_la_SOURCES = emailclient.c
libnstemailclient_la_LDFLAGS = -module -avoid-version
libnstemailclient_la_LIBADD = $(SENDTO_LIBS) $(EMAILCLIENT_LIBS)
