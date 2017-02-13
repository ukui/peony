CLEANFILES = $(NULL)

include $(top_srcdir)/Makefile.shared

lib_LTLIBRARIES=libpeony-extension.la

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(LIBPEONY_EXTENSION_CFLAGS) \
	$(WARNING_CFLAGS) \
	$(DISABLE_DEPRECATED_CFLAGS) \
	-DDATADIR=\""$(datadir)"\" \
	$(UNIQUE_CFLAGS) \
	$(NULL)

libpeony_extension_la_LDFLAGS=\
	-version-info @PEONY_EXTENSION_VERSION_INFO@ \
	-no-undefined \
	$(NULL)

libpeony_extension_la_LIBADD = $(LIBPEONY_EXTENSION_LIBS)

libpeony_extension_includedir=$(includedir)/peony/libpeony-extension

libpeony_extension_include_HEADERS = \
	peony-column-provider.h \
	peony-column.h \
	peony-extension-types.h \
	peony-file-info.h \
	peony-info-provider.h \
	peony-location-widget-provider.h \
	peony-menu-item.h \
	peony-menu-provider.h \
	peony-property-page-provider.h \
	peony-property-page.h \
	peony-menu.h \
	$(NULL)

libpeony_extension_la_SOURCES = \
	peony-column-provider.c \
	peony-column.c \
	peony-extension-i18n.h \
	peony-extension-types.c \
	peony-extension-private.h \
	peony-file-info.c \
	peony-info-provider.c \
	peony-location-widget-provider.c \
	peony-menu-item.c \
	peony-menu-provider.c \
	peony-property-page-provider.c \
	peony-property-page.c \
	peony-menu.c \
	$(NULL)

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=libpeony-extension.pc

EXTRA_DIST = \
	libpeony-extension.pc.in \
	$(NULL)

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_files = \
	$(libpeony_extension_include_HEADERS) \
	$(libpeony_extension_la_SOURCES)
Peony-2.0.gir: libpeony-extension.la Makefile
Peony_2_0_gir_INCLUDES = Gtk-${GTK_API_VERSION} Gio-2.0 GLib-2.0
Peony_2_0_gir_CFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(LIBPEONY_EXTENSION_CFLAGS)
Peony_2_0_gir_LIBS = libpeony-extension.la
Peony_2_0_gir_FILES = $(addprefix $(srcdir)/, $(introspection_files))
INTROSPECTION_GIRS += Peony-2.0.gir

girdir = $(datadir)/gir-1.0/
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)
endif
