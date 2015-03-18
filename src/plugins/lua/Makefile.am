AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libtokentube-plugin-lua.la
libdir = $(prefix)/lib/tokentube/plugins.d

libtokentube_plugin_lua_la_SOURCES = plugin.c script.c
libtokentube_plugin_lua_la_CFLAGS = -I../../../include 
libtokentube_plugin_lua_la_LDFLAGS = -L../../../src/lib/.libs -ltokentube -llua5.2 -lconfuse

