AM_CPPFLAGS =						\
	-DG_LOG_DOMAIN=\"Caja-Image-Converter\"	\
	-DDATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(CAJA_CFLAGS) \
	$(IMAGE_CONVERTER_CFLAGS)

caja_extensiondir=$(CAJA_EXTENSION_DIR)

caja_extension_LTLIBRARIES=libcaja-image-converter.la

libcaja_image_converter_la_SOURCES =				\
	image-converter.c						\
	caja-image-converter.c	caja-image-converter.h	\
	caja-image-resizer.c	caja-image-resizer.h	\
	caja-image-rotator.c	caja-image-rotator.h

libcaja_image_converter_la_LDFLAGS = -module -avoid-version
libcaja_image_converter_la_LIBADD  = $(CAJA_LIBS) $(IMAGE_CONVERTER_LIBS)

builderdir = $(datadir)/caja-extensions
builder_DATA = 				\
	caja-image-resize.ui	\
	caja-image-rotate.ui

extensiondir = $(datadir)/caja/extensions
extension_in_files = libcaja-image-converter.caja-extension.in
extension_DATA = $(extension_in_files:.caja-extension.in=.caja-extension)
%.caja-extension: %.caja-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

EXTRA_DIST = $(builder_DATA)

CLEANFILES = $(extension_DATA)
