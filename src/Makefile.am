include $(top_srcdir)/Makefile.shared

SUBDIRS=file-manager

bin_PROGRAMS = \
	peony \
	peony-file-management-properties \
	peony-autorun-software \
	peony-connect-server \
	$(NULL)

AM_LDFLAGS = \
	`pkg-config --libs udisks2`

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/cut-n-paste-code \
	-I$(top_builddir)/libpeony-private \
	$(CORE_CFLAGS) \
	$(WARNING_CFLAGS) \
	$(EXIF_CFLAGS) \
	$(EXEMPI_CFLAGS) \
	-DDATADIR=\""$(datadir)"\" \
	-DLIBDIR=\""$(libdir)"\" \
	-DPEONY_DATADIR=\""$(datadir)/peony"\" \
	-DUIDIR=\""$(datadir)/peony/ui"\" \
	-DPEONY_PIXMAPDIR=\""$(datadir)/pixmaps/peony"\" \
	-DPREFIX=\""$(prefix)"\" \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DVERSION="\"$(VERSION)\"" \
	$(DISABLE_DEPRECATED_CFLAGS) \
	`pkg-config --cflags udisks2` \
	$(NULL)

LDADD = \
	$(top_builddir)/src/file-manager/libpeony-file-manager.la \
	$(top_builddir)/libpeony-private/libpeony-private.la \
	$(CORE_LIBS) \
	$(EXIF_LIBS) \
	$(EXEMPI_LIBS) \
	$(POPT_LIBS) \
    -lnotify
	$(NULL)

dbus_freedesktop_built_sources =			\
	peony-freedesktop-generated.c		\
	peony-freedesktop-generated.h

$(dbus_freedesktop_built_sources) : Makefile.am $(top_srcdir)/data/freedesktop-dbus-interfaces.xml
	gdbus-codegen									\
		--interface-prefix org.freedesktop.					\
		--c-namespace PeonyFreedesktop					\
		--c-generate-object-manager						\
		--generate-c-code peony-freedesktop-generated			\
		$(top_srcdir)/data/freedesktop-dbus-interfaces.xml			\
		$(NULL)

@INTLTOOL_DESKTOP_RULE@

desktop_in_files=ukui-network-scheme.desktop.in
desktop_files=$(desktop_in_files:.desktop.in=.desktop)
desktopdir = $(datadir)/ukui/network/

schemedir = $(datadir)/applications
scheme_DATA = ukui-network-scheme.desktop

BUILT_SOURCES = \
	peony-src-marshal.c \
	peony-src-marshal.h \
	$(dbus_freedesktop_built_sources) \
	$(NULL)
peony_SOURCES = \
	peony-actions.h \
	peony-application.c \
	peony-application.h \
	peony-bookmark-list.c \
	peony-bookmark-list.h \
	peony-bookmarks-window.c \
	peony-bookmarks-window.h \
	peony-connect-server-dialog.c \
	peony-connect-server-dialog.h \
	peony-connect-server-dialog-nonmain.c \
	peony-connect-server-operation.c	\
	peony-connect-server-operation.h	\
	peony-desktop-window.c \
	peony-desktop-window.h \
	peony-emblem-sidebar.c \
	peony-emblem-sidebar.h \
	peony-file-management-properties.c \
	peony-file-management-properties.h \
	peony-freedesktop-dbus.c \
	peony-freedesktop-dbus.h \
	peony-history-sidebar.c \
	peony-history-sidebar.h \
	peony-image-properties-page.c \
	peony-image-properties-page.h \
	peony-information-panel.c \
	peony-information-panel.h \
	peony-kdisk-format.h \
	peony-kdisk-format.c \
	peony-kdisk-createformat.h \
	peony-kdisk-createformat.c \
	peony-location-bar.c \
	peony-location-bar.h \
	peony-location-dialog.c \
	peony-location-dialog.h \
	peony-location-entry.c \
	peony-location-entry.h \
	peony-main.c \
	peony-navigation-action.c \
	peony-navigation-action.h \
	peony-navigation-window-menus.c \
	peony-navigation-window.c \
	peony-navigation-window.h \
	peony-navigation-window-pane.c \
	peony-navigation-window-pane.h \
	peony-navigation-window-slot.c \
	peony-navigation-window-slot.h \
	peony-notebook.c \
	peony-notebook.h \
	peony-notes-viewer.c \
	peony-notes-viewer.h \
	peony-pathbar.c \
	peony-pathbar.h \
	peony-places-sidebar.c \
	peony-places-sidebar.h \
	peony-property-browser.c \
	peony-property-browser.h \
	peony-query-editor.c \
	peony-query-editor.h \
	peony-search-bar.c \
	peony-search-bar.h \
	peony-self-check-functions.c \
	peony-self-check-functions.h \
	peony-side-pane.c \
	peony-side-pane.h \
	peony-sidebar-title.c \
	peony-sidebar-title.h \
	peony-spatial-window.c \
	peony-spatial-window.h \
	peony-trash-bar.c \
	peony-trash-bar.h \
	peony-view-as-action.c \
	peony-view-as-action.h \
	peony-window-bookmarks.c \
	peony-window-bookmarks.h \
	peony-window-manage-views.c \
	peony-window-manage-views.h \
	peony-window-menus.c \
	peony-window-pane.c \
	peony-window-pane.h \
	peony-window-private.h \
	peony-window-slot.c \
	peony-window-slot.h \
	peony-window-toolbars.c \
	peony-window.c \
	peony-window.h \
	peony-x-content-bar.c \
	peony-x-content-bar.h \
	peony-zoom-action.c \
	peony-zoom-action.h \
	peony-zoom-control.c \
	peony-zoom-control.h \
	$(NULL)

nodist_peony_SOURCES = \
	$(BUILT_SOURCES) \
	$(NULL)

peony-src-marshal.list: $(peony_SOURCES) Makefile.am
	$(AM_V_GEN)( cd $(srcdir) && \
	sed -n -e 's/.*peony_src_marshal_\([[:upper:][:digit:]]*__[[:upper:][:digit:]_]*\).*/\1/p' \
	$(peony_SOURCES) ) \
	| sed -e 's/__/:/' -e 'y/_/,/' | sort -u > $@.tmp
	@if cmp -s $@.tmp $@; then \
		rm $@.tmp; \
	else \
		mv $@.tmp $@; \
	fi

%-marshal.c: %-marshal.list Makefile
	$(AM_V_GEN)echo "#include \"peony-src-marshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) --body --prefix=$(subst -,_,$*)_marshal $< >> $*-marshal.c

%-marshal.h: %-marshal.list Makefile
	$(AM_V_GEN)$(GLIB_GENMARSHAL) --header --prefix=$(subst -,_,$*)_marshal $< > $*-marshal.h

peony_file_management_properties_SOURCES = \
	peony-file-management-properties.c \
	peony-file-management-properties.h \
	peony-file-management-properties-main.c \
	$(NULL)

peony_autorun_software_SOURCES = \
	peony-autorun-software.c \
	$(NULL)

peony_connect_server_SOURCES = \
	peony-bookmark-list.c \
	peony-bookmark-list.h \
	peony-connect-server-dialog.c \
	peony-connect-server-dialog.h \
	peony-connect-server-dialog-main.c \
	peony-connect-server-operation.c     \
	peony-connect-server-operation.h     \
	$(NULL)


TESTS=check-peony

@INTLTOOL_SERVER_RULE@

uidir = $(datadir)/peony/ui
ui_DATA = \
	peony-shell-ui.xml \
	peony-navigation-window-ui.xml \
	peony-spatial-window-ui.xml \
	peony-file-management-properties.ui \
	peony-bookmarks-window.ui \
	$(NULL)

CLEANFILES = \
	peony-src-marshal.list \
	$(BUILT_SOURCES) \
	$(desktop_files) \
	$(server_DATA) \
	$(NULL)

EXTRA_DIST = \
	peony-src-marshal.list \
	$(server_in_files) \
	$(ui_DATA) \
	check-peony \
	$(desktop_in_files) \
	$(NULL)

dist-hook:
	cd $(distdir); rm -f $(CLEANFILES)
