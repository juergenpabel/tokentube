AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libtokentube.la
libdir = $(prefix)/lib

libtokentube_la_CFLAGS = -I../../include 
libtokentube_la_LDFLAGS = -lext2fs -lscrypt -lgcrypt -lconfuse -ldl
libtokentube_la_SOURCES = crypto.c crypto-conf.c crypto-crypt.c crypto-oid.c init.c library.c plugin.c plugin-event.c plugin-pba.c plugin-runtime.c plugin-storage.c plugin-user.c plugin-otp.c plugin-auth.c default.c default-auth.c default-storage-posix.c default-storage-ext2fs.c default-storage-misc.c default-pba.c default-runtime.c default-storage.c default-user.c default-user-storage.c default-user-key.c default-user-hmac.c default-user-events.c default-otp.c default-helpdesk.c default-helpdesk-storage.c default-helpdesk-events.c default-otp-storage.c runtime.c runtime-conf.c runtime-conf-confuse.c runtime-event.c runtime-log.c runtime-debug.c runtime-pba.c util-base32.c util-base64.c util-crc.c util-device.c util-file.c util-hex.c

