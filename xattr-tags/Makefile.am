AM_CPPFLAGS = $(PEONY_CFLAGS)

peony_extensiondir = $(PEONY_EXTENSION_DIR)
peony_extension_LTLIBRARIES = libpeony-xattr-tags.la

libpeony_xattr_tags_la_SOURCES = \
	peony-xattr-tags-extension.c \
	peony-xattr-tags-extension.h

libpeony_xattr_tags_la_LDFLAGS = -module -avoid-version
libpeony_xattr_tags_la_LIBADD = $(PEONY_LIBS)

extensiondir = $(datadir)/peony/extensions
extension_in_files = libpeony-xattr-tags.peony-extension.in
extension_DATA = $(extension_in_files:.peony-extension.in=.peony-extension)
%.peony-extension: %.peony-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

CLEANFILES = $(extension_DATA)
