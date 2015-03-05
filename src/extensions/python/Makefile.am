AUTOMAKE_OPTIONS = foreign

pyexec_LTLIBRARIES = tokentube.la

override LDFLAGS =

tokentube_la_SOURCES = extension.c user.c otp.c
tokentube_la_CFLAGS = -I../../../include -I/usr/include/python3.4m
tokentube_la_LDFLAGS = -L../../../src/lib/.libs -ltokentube -module -avoid-version -export-symbols-regex PyInit_tokentube

