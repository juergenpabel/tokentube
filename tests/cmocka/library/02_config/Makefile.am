AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = test

test_CFLAGS  = -I../../../../include -Wl,--wrap=fprintf
test_LDFLAGS = -lcmocka -lgcrypt -lconfuse
test_SOURCES = test.c internal.c ../../../../src/lib/init.c ../../../../src/lib/runtime-conf.c ../../../../src/lib/runtime-conf-confuse.c ../../../../src/lib/util-hex.c

.PHONY:	tests
tests:
	./test

