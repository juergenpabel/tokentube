AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libtokentube-plugin-test.la
libdir = $(prefix)/lib/tokentube/plugins.d

libtokentube_plugin_test_la_SOURCES = plugin.c posix.c misc.c
libtokentube_plugin_test_la_CFLAGS = -I../../../include 
libtokentube_plugin_test_la_LDFLAGS = -L../../../src/lib/.libs -ltokentube

