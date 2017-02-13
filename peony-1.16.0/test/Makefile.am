NULL=

AM_CPPFLAGS =\
	-I$(top_srcdir) \
	$(CORE_CFLAGS) \
	$(WARNING_CFLAGS) \
	-DVERSION="\"$(VERSION)\"" \
	-DPEONY_DATADIR=\""$(datadir)/peony"\" \
	-DUKUILOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\" \
	$(NULL)

LDADD =\
	$(top_builddir)/libpeony-private/libpeony-private.la \
	$(CORE_LIBS) \
	$(NULL)

noinst_PROGRAMS =\
	test-peony-wrap-table \
	test-peony-search-engine \
	test-peony-directory-async \
	test-peony-copy \
	test-eel-background \
	test-eel-editable-label \
	test-eel-image-table \
	test-eel-labeled-image \
	test-eel-pixbuf-scale \
	$(NULL)

test_peony_copy_SOURCES = test-copy.c test.c

test_peony_wrap_table_SOURCES = test-peony-wrap-table.c test.c

test_peony_search_engine_SOURCES = test-peony-search-engine.c 

test_peony_directory_async_SOURCES = test-peony-directory-async.c

test_eel_background_SOURCES = test-eel-background.c
test_eel_image_table_SOURCES = test-eel-image-table.c test.c
test_eel_labeled_image_SOURCES = test-eel-labeled-image.c test.c test.h
test_eel_pixbuf_scale_SOURCES = test-eel-pixbuf-scale.c test.c test.h

EXTRA_DIST = \
	test.h \
	$(NULL)
