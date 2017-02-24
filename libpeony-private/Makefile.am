include $(top_srcdir)/Makefile.shared

noinst_LTLIBRARIES=libpeony-private.la

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_srcdir)/cut-n-paste-code \
	$(CORE_CFLAGS) \
	$(WARNING_CFLAGS) \
	$(DISABLE_DEPRECATED_CFLAGS) \
	$(TRACKER_CFLAGS) \
	$(BEAGLE_CFLAGS) \
	-DDATADIR=\""$(datadir)"\" \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DPEONY_DATADIR=\""$(datadir)/peony"\" \
	-DPEONY_EXTENSIONDIR=\""$(libdir)/peony/extensions-2.0"\" \
	$(NULL)

BUILT_SOURCES = \
	peony-marshal.c \
	peony-marshal.h \
	$(NULL)

dependency_static_libs = \
	$(top_builddir)/cut-n-paste-code/libegg/libegg.la \
	$(NULL)

libpeony_private_la_LDFLAGS = \
	-no-undefined \
	$(NULL)

libpeony_private_la_LIBADD = \
	$(dependency_static_libs) \
	$(SELINUX_LIBS) \
	$(BEAGLE_LIBS) \
	$(TRACKER_LIBS) \
	$(top_builddir)/eel/libeel-2.la \
	$(top_builddir)/libpeony-extension/libpeony-extension.la \
	$(CORE_LIBS) \
    -lnotify
	$(NULL)

libpeony_private_la_SOURCES = \
	peony-autorun.c \
	peony-autorun.h \
	peony-bookmark.c \
	peony-bookmark.h \
	peony-cell-renderer-text-ellipsized.c \
	peony-cell-renderer-text-ellipsized.h \
	peony-clipboard-monitor.c \
	peony-clipboard-monitor.h \
	peony-clipboard.c \
	peony-clipboard.h \
	peony-column-chooser.c \
	peony-column-chooser.h \
	peony-column-utilities.c \
	peony-column-utilities.h \
	peony-customization-data.c \
	peony-customization-data.h \
	peony-debug-log.c \
	peony-debug-log.h \
	peony-default-file-icon.c \
	peony-default-file-icon.h \
	peony-desktop-directory-file.c \
	peony-desktop-directory-file.h \
	peony-desktop-directory.c \
	peony-desktop-directory.h \
	peony-desktop-icon-file.c \
	peony-desktop-icon-file.h \
	peony-desktop-link-monitor.c \
	peony-desktop-link-monitor.h \
	peony-desktop-link.c \
	peony-desktop-link.h \
	peony-desktop-metadata.c \
	peony-desktop-metadata.h \
	peony-directory-async.c \
	peony-directory-background.c \
	peony-directory-background.h \
	peony-directory-notify.h \
	peony-directory-private.h \
	peony-directory.c \
	peony-directory.h \
	peony-dnd.c \
	peony-dnd.h \
	peony-emblem-utils.c \
	peony-emblem-utils.h \
	peony-extensions.c \
	peony-extensions.h \
	peony-entry.c \
	peony-entry.h \
	peony-file-attributes.h \
	peony-file-changes-queue.c \
	peony-file-changes-queue.h \
	peony-file-conflict-dialog.c \
	peony-file-conflict-dialog.h \
	peony-file-dnd.c \
	peony-file-dnd.h \
	peony-file-operations.c \
	peony-file-operations.h \
	peony-file-private.h \
	peony-file-queue.c \
	peony-file-queue.h \
	peony-file-utilities.c \
	peony-file-utilities.h \
	peony-file.c \
	peony-file.h \
	peony-global-preferences.c \
	peony-global-preferences.h \
	peony-icon-canvas-item.c \
	peony-icon-canvas-item.h \
	peony-icon-container.c \
	peony-icon-container.h \
	peony-icon-dnd.c \
	peony-icon-dnd.h \
	peony-icon-private.h \
	peony-icon-info.c \
	peony-icon-info.h \
	peony-icon-names.h \
	peony-keep-last-vertical-box.c \
	peony-keep-last-vertical-box.h \
	peony-lib-self-check-functions.c \
	peony-lib-self-check-functions.h \
	peony-link.c \
	peony-link.h \
	peony-metadata.h \
	peony-metadata.c \
	peony-mime-actions.c \
	peony-mime-actions.h \
	peony-mime-application-chooser.c \
	peony-mime-application-chooser.h \
	peony-module.c \
	peony-module.h \
	peony-monitor.c \
	peony-monitor.h \
	peony-open-with-dialog.c \
	peony-open-with-dialog.h \
	peony-progress-info.c \
	peony-progress-info.h \
	peony-program-choosing.c \
	peony-program-choosing.h \
	peony-recent.c \
	peony-recent.h \
	peony-saved-search-file.c \
	peony-saved-search-file.h \
	peony-search-directory.c \
	peony-search-directory.h \
	peony-search-directory-file.c \
	peony-search-directory-file.h \
	peony-search-engine.c \
	peony-search-engine.h \
	peony-search-engine-simple.c \
	peony-search-engine-simple.h \
	peony-search-engine-beagle.c \
	peony-search-engine-beagle.h \
	peony-search-engine-tracker.c \
	peony-search-engine-tracker.h \
	peony-sidebar-provider.c \
	peony-sidebar-provider.h \
	peony-sidebar.c \
	peony-sidebar.h \
	peony-signaller.h \
	peony-signaller.c \
	peony-query.c \
	peony-query.h \
	peony-thumbnails.c \
	peony-thumbnails.h \
	peony-trash-monitor.c \
	peony-trash-monitor.h \
	peony-tree-view-drag-dest.c \
	peony-tree-view-drag-dest.h \
	peony-ui-utilities.c \
	peony-ui-utilities.h \
	peony-vfs-directory.c \
	peony-vfs-directory.h \
	peony-vfs-file.c \
	peony-vfs-file.h \
	peony-view-factory.c \
	peony-view-factory.h \
	peony-view.c \
	peony-view.h \
	peony-window-info.c \
	peony-window-info.h \
	peony-window-slot-info.c \
	peony-window-slot-info.h \
	peony-undostack-manager.c \
	peony-undostack-manager.h \
	$(NULL)

nodist_libpeony_private_la_SOURCES =\
	$(BUILT_SOURCES) \
	$(NULL)

$(lib_LTLIBRARIES): $(dependency_static_libs)

peony-marshal.list: $(libpeony_private_la_SOURCES) Makefile.am
	$(AM_V_GEN)( cd $(srcdir) && \
	sed -n -e 's/.*peony_marshal_\([[:upper:][:digit:]]*__[[:upper:][:digit:]_]*\).*/\1/p' \
	$(libpeony_private_la_SOURCES) ) \
	| sed -e 's/__/:/' -e 'y/_/,/' | sort -u > $@.tmp
	@if cmp -s $@.tmp $@; then \
		rm $@.tmp; \
	else \
		mv $@.tmp $@; \
	fi

%-marshal.h: %-marshal.list Makefile
	$(AM_V_GEN)$(GLIB_GENMARSHAL) --header --prefix=$(subst -,_,$*)_marshal $< > $*-marshal.h

%-marshal.c: %-marshal.list Makefile
	$(AM_V_GEN)echo "#include \"peony-marshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) --body --prefix=$(subst -,_,$*)_marshal $< >> $*-marshal.c

gsettings_SCHEMAS = org.ukui.peony.gschema.xml org.ukui.media-handling.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

EXTRA_DIST = \
	peony-marshal.list \
	$(gsettings_SCHEMAS) \
	$(BEAGLE_SOURCES) \
	$(TRACKER_SOURCES) \
	$(NULL)

CLEANFILES = \
	$(BUILT_SOURCES) \
	peony-marshal.list \
	$(NULL)

dist-hook:
	cd $(distdir); rm -f $(CLEANFILES)

