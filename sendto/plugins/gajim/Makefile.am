plugindir = $(libdir)/caja-sendto/plugins

AM_CPPFLAGS =                                      \
        -DDATADIR=\"$(datadir)\"                \
        -DICONDIR=\"$(icondir)\"                \
	-DLOCALEDIR="\"$(datadir)/locale\""	\
        -I$(top_srcdir)/sendto                  \
        -I$(top_builddir)                       \
        $(SENDTO_CFLAGS)               \
	$(DBUS_CFLAGS)                          \
        $(DISABLE_DEPRECATED)			\
	$(WARN_CFLAGS)

plugin_LTLIBRARIES = libnstgajim.la

libnstgajim_la_SOURCES = gajim.c
libnstgajim_la_LDFLAGS = -module -avoid-version
libnstgajim_la_LIBADD = $(DBUS_LIBS) $(SENDTO_LIBS)

