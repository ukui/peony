interfacesdir = $(datadir)/caja-extensions
interfaces_DATA = share-dialog.ui

libcaja_share_la_CFLAGS =				\
	-Wall						\
	-DG_LOG_DOMAIN=\"Caja-Share\"		\
	-DINTERFACES_DIR=\"$(interfacesdir)\"		\
	-DDATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(CAJA_CFLAGS)				\
	$(SHARE_CFLAGS)

caja_extensiondir=$(CAJA_EXTENSION_DIR)
caja_extension_LTLIBRARIES=libcaja-share.la

libcaja_share_la_SOURCES =  \
	caja-share.c        \
	caja-share.h	\
	shares.c		\
	shares.h

libcaja_share_la_LDFLAGS = -module -avoid-version
libcaja_share_la_LIBADD  = $(SHARE_LIBS) $(CAJA_LIBS)

extensiondir = $(datadir)/caja/extensions
extension_in_files = libcaja-share.caja-extension.in
extension_DATA = $(extension_in_files:.caja-extension.in=.caja-extension)
%.caja-extension: %.caja-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

EXTRA_DIST = share-dialog.ui

CLEANFILES = $(extension_DATA)
