interfacesdir = $(datadir)/peony-extensions
interfaces_DATA = share-dialog.ui

libpeony_share_la_CFLAGS =				\
	-Wall						\
	-DG_LOG_DOMAIN=\"Peony-Share\"		\
	-DINTERFACES_DIR=\"$(interfacesdir)\"		\
	-DDATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(PEONY_CFLAGS)				\
	$(SHARE_CFLAGS)

peony_extensiondir=$(PEONY_EXTENSION_DIR)
peony_extension_LTLIBRARIES=libpeony-share.la

libpeony_share_la_SOURCES =  \
	peony-share.c        \
	peony-share.h	\
	shares.c		\
	shares.h

libpeony_share_la_LDFLAGS = -module -avoid-version
libpeony_share_la_LIBADD  = $(SHARE_LIBS) $(PEONY_LIBS)

extensiondir = $(datadir)/peony/extensions
extension_in_files = libpeony-share.peony-extension.in
extension_DATA = $(extension_in_files:.peony-extension.in=.peony-extension)
%.peony-extension: %.peony-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

EXTRA_DIST = share-dialog.ui

CLEANFILES = $(extension_DATA)
