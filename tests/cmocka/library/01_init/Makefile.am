AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = test

test_SOURCES = main.c internal.c ../../../../src/lib/init.c
test_CFLAGS  = -I../../../../include -Wl,--wrap=fprintf
test_LDFLAGS = -lcmocka

.PHONY:	tests
tests:
	./test
