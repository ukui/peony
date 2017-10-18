AM_CPPFLAGS =						\
	-DG_LOG_DOMAIN=\"Peony-Open-Terminal\"	\
	-DDATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" 	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	$(WARN_CFLAGS)                                  \
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(PEONY_CFLAGS)				\
	$(OPENTERMINAL_CFLAGS)

peony_extensiondir=$(PEONY_EXTENSION_DIR)

peony_extension_LTLIBRARIES=libpeony-open-terminal.la

libpeony_open_terminal_la_SOURCES = \
	peony-open-terminal.c       \
	peony-open-terminal.h       \
	open-terminal.c

libpeony_open_terminal_la_LDFLAGS = -module -avoid-version
libpeony_open_terminal_la_LIBADD  = $(PEONY_LIBS) $(OPENTERMINAL_LIBS)

gsettings_schemas_in_files = org.mate.peony-open-terminal.gschema.xml.in
gsettings_SCHEMAS = $(gsettings_schemas_in_files:.gschema.xml.in=.gschema.xml)

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

extensiondir = $(datadir)/peony/extensions
extension_in_files = libpeony-open-terminal.peony-extension.in
extension_DATA = $(extension_in_files:.peony-extension.in=.peony-extension)
%.peony-extension: %.peony-extension.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

DISTCLEANFILES = \
	org.mate.peony-open-terminal.gschema.xml

CLEANFILES = $(extension_DATA)

EXTRA_DIST = \
	$(gsettings_schemas_in_files)
