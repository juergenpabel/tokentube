AUTOMAKE_OPTIONS = foreign

pyexec_LTLIBRARIES = tokentube.la

override CFLAGS =
override CPPFLAGS =
override LDFLAGS =

tokentube_la_SOURCES = binding.c user.c otp.c auth.c runtime.c
tokentube_la_CFLAGS = -I../../../include -I/usr/include/python3.4m
tokentube_la_LDFLAGS = -L../../../src/lib/.libs -lpython3.4m -ltokentube -module -avoid-version -export-symbols-regex PyInit_tokentube

