plugindir = $(libdir)/caja-sendto/plugins

AM_CPPFLAGS =                                      \
        -DDATADIR=\"$(datadir)\"                \
        -DICONDIR=\"$(icondir)\"                \
	-DLOCALEDIR="\"$(datadir)/locale\""	\
        -I$(top_srcdir)/sendto                  \
        -I$(top_builddir)                       \
        $(SENDTO_CFLAGS)               \
	$(NST_EBOOK_CFLAGS)                     \
	$(PIDGIN_CFLAGS)                        \
	$(DISABLE_DEPRECATED)			\
	$(WARN_CFLAGS)

plugin_LTLIBRARIES = libnstpidgin.la

libnstpidgin_la_SOURCES = pidgin.c
libnstpidgin_la_LDFLAGS = -module -avoid-version
libnstpidgin_la_LIBADD = $(PIDGIN_LIBS) $(SENDTO_LIBS)

