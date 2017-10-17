AM_CFLAGS = -g -O2 -Wall
AM_CPPFLAGS = -DLOCALEDIR=\"$(datadir)/locale\" -DDATA_DIR=\"$(datadir)\" -DPREFIX=\"$(prefix)\" $(CAJA_CFLAGS)

caja_extensiondir=$(libdir)/caja/extensions-2.0
caja_extension_LTLIBRARIES = libcaja-gksu.la
libcaja_gksu_la_SOURCES = libcaja-gksu.c
libcaja_gksu_la_LDFLAGS = -module -avoid-version -no-undefined
libcaja_gksu_la_LIBADD  = $(CAJA_LIBS)

extensiondir = $(datadir)/caja/extensions
extension_in_files = libcaja-gksu.caja-extension.in
extension_DATA = $(extension_in_files:.caja-extension.in=.caja-extension)
%.caja-extension: %.caja-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

CLEANFILES = $(extension_DATA)
