AUTOMAKE_OPTIONS = foreign

libexec_PROGRAMS = pba
libexecdir = $(prefix)/lib/tokentube/libexec

pba_SOURCES = pba.c pba-init.c pba-plain.c pba-user.c pba-ssod.c pba-otp.c pba-plymouth-splash.c pba-plymouth-plain.c pba-plymouth-user.c pba-plymouth-otp.c
pba_CFLAGS = -Wno-unused-result -I../../../include -I../../lib -I/usr/include/plymouth-1/ply
pba_LDFLAGS = -L../../lib/ -lply -lply-boot-client -lconfuse -ltokentube
