AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = tokentube.la
libdir = $(prefix)/usr/lib/python3/dist-packages

tokentube_la_SOURCES = extension.c user.c otp.c
tokentube_la_CFLAGS = -I../../../include -I/usr/include/python3.4
tokentube_la_LDFLAGS = -L../../../src/lib/.libs -ltokentube -module -avoid-version -export-symbols-regex PyInit_tokentube

