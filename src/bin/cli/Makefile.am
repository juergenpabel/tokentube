AUTOMAKE_OPTIONS = foreign

sbin_PROGRAMS = tokentube
sbindir = $(prefix)/sbin

tokentube_CFLAGS  = -I../../../include
tokentube_LDFLAGS = -L../../lib -ltokentube -lgcrypt
tokentube_SOURCES = tokentube.c

