AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libtokentube-plugin-syslog.la
libdir = $(prefix)/usr/lib/tokentube/plugins.d

libtokentube_plugin_syslog_la_SOURCES = plugin.c
libtokentube_plugin_syslog_la_CFLAGS = -I../../../include 
libtokentube_plugin_syslog_la_LDFLAGS = -L../../../src/lib/.libs -ltokentube

