AM_CPPFLAGS =						\
	-DFR_DATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(PEONY_CFLAGS)

peony_extensiondir=$(PEONY_EXTENSION_DIR)

peony_extension_LTLIBRARIES=libpeony-parchives.la

libpeony_parchives_la_SOURCES =     \
	peony-parchives.c           \
	peony-parchives.h           \
	parchives-module.c

libpeony_parchives_la_LDFLAGS = -module -avoid-version -no-undefined
libpeony_parchives_la_LIBADD  = $(PEONY_LIBS)

extensiondir = $(datadir)/peony/extensions
extension_in_files = libpeony-parchives.peony-extension.in
extension_DATA = $(extension_in_files:.peony-extension.in=.peony-extension)
%.peony-extension: %.peony-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

CLEANFILES = libpeony-parchives.peony-extension

EXTRA_DIST = $(extension_in_files)
-include $(top_srcdir)/git.mk
