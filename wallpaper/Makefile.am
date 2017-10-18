AM_CPPFLAGS = $(PEONY_CFLAGS)

peony_extensiondir = $(PEONY_EXTENSION_DIR)
peony_extension_LTLIBRARIES = libpeony-wallpaper.la

libpeony_wallpaper_la_SOURCES = \
	peony-wallpaper-extension.c\
	peony-wallpaper-extension.h\
	peony-wallpaper-command.c

libpeony_wallpaper_la_LDFLAGS = -module -avoid-version
libpeony_wallpaper_la_LIBADD = $(PEONY_LIBS)

extensiondir = $(datadir)/peony/extensions
extension_in_files = libpeony-wallpaper.peony-extension.in
extension_DATA = $(extension_in_files:.peony-extension.in=.peony-extension)
%.peony-extension: %.peony-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

CLEANFILES = $(extension_DATA)
