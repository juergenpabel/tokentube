AUTOMAKE_OPTIONS = foreign

securelib_LTLIBRARIES = pam_tokentube.la
securelibdir = $(prefix)/lib/security

pam_tokentube_la_CFLAGS = -I../../include -I../lib
pam_tokentube_la_LDFLAGS = -L../lib -lpam -ltokentube -module -avoid-version
pam_tokentube_la_SOURCES = pam_tokentube.c

