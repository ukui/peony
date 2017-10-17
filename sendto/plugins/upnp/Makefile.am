plugindir = $(libdir)/caja-sendto/plugins

AM_CPPFLAGS =                                      \
        -DDATADIR=\"$(datadir)\"                \
        -DICONDIR=\"$(icondir)\"                \
	-DLOCALEDIR="\"$(datadir)/locale\""	\
        -I$(top_srcdir)/sendto                  \
        -I$(top_builddir)                       \
        $(SENDTO_CFLAGS)               \
	$(UPNP_CFLAGS)                          \
	$(DISABLE_DEPRECATED)			\
	$(WARN_CFLAGS)

plugin_LTLIBRARIES = libnstupnp.la

libnstupnp_la_SOURCES = upnp.c
libnstupnp_la_LDFLAGS = -module -avoid-version
libnstupnp_la_LIBADD = $(UPNP_LIBS) $(SENDTO_LIBS)

