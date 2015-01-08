AUTOMAKE_OPTIONS = foreign

libexec_PROGRAMS = pba
libexecdir = $(prefix)/usr/lib/tokentube/libexec

pba_SOURCES = pba.c pba-init.c pba-user.c pba-ssod.c pba-otp.c pba-plymouth.c pba-otp-plymouth.c
pba_CFLAGS = -Wno-unused-result -I../../../include -I../../lib -I/usr/include/plymouth-1/ply
pba_LDFLAGS = -L../../lib/ -lply -lply-boot-client -lconfuse -ltokentube
