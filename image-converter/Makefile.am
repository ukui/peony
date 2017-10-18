AM_CPPFLAGS =						\
	-DG_LOG_DOMAIN=\"Peony-Image-Converter\"	\
	-DDATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(PEONY_CFLAGS) \
	$(IMAGE_CONVERTER_CFLAGS)

peony_extensiondir=$(PEONY_EXTENSION_DIR)

peony_extension_LTLIBRARIES=libpeony-image-converter.la

libpeony_image_converter_la_SOURCES =				\
	image-converter.c						\
	peony-image-converter.c	peony-image-converter.h	\
	peony-image-resizer.c	peony-image-resizer.h	\
	peony-image-rotator.c	peony-image-rotator.h

libpeony_image_converter_la_LDFLAGS = -module -avoid-version
libpeony_image_converter_la_LIBADD  = $(PEONY_LIBS) $(IMAGE_CONVERTER_LIBS)

builderdir = $(datadir)/peony-extensions
builder_DATA = 				\
	peony-image-resize.ui	\
	peony-image-rotate.ui

extensiondir = $(datadir)/peony/extensions
extension_in_files = libpeony-image-converter.peony-extension.in
extension_DATA = $(extension_in_files:.peony-extension.in=.peony-extension)
%.peony-extension: %.peony-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

EXTRA_DIST = $(builder_DATA)

CLEANFILES = $(extension_DATA)
