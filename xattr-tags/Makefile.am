AM_CPPFLAGS = $(CAJA_CFLAGS)

caja_extensiondir = $(CAJA_EXTENSION_DIR)
caja_extension_LTLIBRARIES = libcaja-xattr-tags.la

libcaja_xattr_tags_la_SOURCES = \
	caja-xattr-tags-extension.c \
	caja-xattr-tags-extension.h

libcaja_xattr_tags_la_LDFLAGS = -module -avoid-version
libcaja_xattr_tags_la_LIBADD = $(CAJA_LIBS)

extensiondir = $(datadir)/caja/extensions
extension_in_files = libcaja-xattr-tags.caja-extension.in
extension_DATA = $(extension_in_files:.caja-extension.in=.caja-extension)
%.caja-extension: %.caja-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

CLEANFILES = $(extension_DATA)
