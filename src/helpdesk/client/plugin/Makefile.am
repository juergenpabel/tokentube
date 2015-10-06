AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libtokentube-plugin-helpdesk.la
libdir = $(prefix)/lib/tokentube/plugins.d

libtokentube_plugin_helpdesk_la_SOURCES = plugin.c storage.c storage-smtp.c storage-rest.c
libtokentube_plugin_helpdesk_la_CFLAGS = -I../../../include 
libtokentube_plugin_helpdesk_la_LDFLAGS = -L../../../src/lib/.libs -ltokentube -lgcrypt -lconfuse -lgpgme -lesmtp -lresolv -lcurl

