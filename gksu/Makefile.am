AM_CFLAGS = -g -O2 -Wall
AM_CPPFLAGS = -DLOCALEDIR=\"$(datadir)/locale\" -DDATA_DIR=\"$(datadir)\" -DPREFIX=\"$(prefix)\" $(PEONY_CFLAGS)

peony_extensiondir=$(libdir)/peony/extensions-2.0
peony_extension_LTLIBRARIES = libpeony-gksu.la
libpeony_gksu_la_SOURCES = libpeony-gksu.c
libpeony_gksu_la_LDFLAGS = -module -avoid-version -no-undefined
libpeony_gksu_la_LIBADD  = $(PEONY_LIBS)

extensiondir = $(datadir)/peony/extensions
extension_in_files = libpeony-gksu.peony-extension.in
extension_DATA = $(extension_in_files:.peony-extension.in=.peony-extension)
%.peony-extension: %.peony-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

CLEANFILES = $(extension_DATA)
