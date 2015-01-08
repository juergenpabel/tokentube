AUTOMAKE_OPTIONS = foreign

sbin_PROGRAMS = tokentube
sbindir = $(prefix)/usr/sbin

tokentube_CFLAGS  = -I../../../include
tokentube_LDFLAGS = -L../../lib -ltokentube -lgcrypt
tokentube_SOURCES = tokentube.c

