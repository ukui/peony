NULL =

xml_in_files = peony.xml.in
xml_files = $(xml_in_files:.xml.in=.xml)
@INTLTOOL_XML_RULE@

desktopdir = $(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
desktop_in_files = \
	peony.desktop.in \
	peony-home.desktop.in \
	peony-computer.desktop.in \
	peony-folder-handler.desktop.in \
	peony-file-management-properties.desktop.in  \
	peony-browser.desktop.in                     \
	peony-autorun-software.desktop.in
@INTLTOOL_DESKTOP_RULE@

mimedir = $(datadir)/mime/packages
mime_DATA = $(xml_files)

servicedir = $(datadir)/dbus-1/services
service_DATA = $(service_in_files:.service.in=.service)
service_in_files = 				\
	org.ukui.freedesktop.FileManager1.service.in

org.ukui.freedesktop.FileManager1.service: org.ukui.freedesktop.FileManager1.service.in Makefile
	$(AM_V_GEN) sed -e "s|\@bindir\@|$(bindir)|" $< > $@

peonydatadir = $(datadir)/peony

peonydata_DATA = \
	browser.xml \
	peony-extras.placeholder  \
	peony-suggested.placeholder \
	peony.css \
    peony-desktop.css \
	$(NULL)

# app data file
appdatadir = $(datadir)/appdata
appdata_in_files = peony.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)

EXTRA_DIST = $(peonydata_DATA) \
	peony.desktop \
	peony.desktop.in \
	peony.css \
	freedesktop-dbus-interfaces.xml \
	$(xml_in_files) \
	$(desktop_in_files) \
	$(service_in_files) \
	$(appdata_in_files) \
	$(NULL)

CLEANFILES = $(xml_files) \
	$(desktop_DATA) \
	$(service_DATA) \
	peony.appdata.xml \
	$(NULL)

SUBDIRS = \
	icons \
	patterns \
	$(NULL)

install-data-hook:
if ENABLE_UPDATE_MIMEDB
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(datadir)/mime"
endif

uninstall-hook:
if ENABLE_UPDATE_MIMEDB
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(datadir)/mime"
endif
