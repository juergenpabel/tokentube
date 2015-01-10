AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = test

test_SOURCES = test.c
test_CFLAGS  = -I../../../../include
test_LDFLAGS = -L../../../../src/lib/.libs -ltokentube -lcmocka

.PHONY:	tests
tests:
	./test

