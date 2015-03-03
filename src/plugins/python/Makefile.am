AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libtokentube-plugin-python.la
libdir = $(prefix)/usr/lib/tokentube/plugins.d

libtokentube_plugin_python_la_SOURCES = plugin.c python.c luks.c
libtokentube_plugin_python_la_CFLAGS = -I../../../include -I/usr/include/python3.4
libtokentube_plugin_python_la_LDFLAGS = -L../../../src/lib/.libs -ltokentube -lconfuse -lpython3.4m

