AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = test

test_SOURCES = main.c internal.c
test_CFLAGS  = -I../../../../include -I../../../../src/lib
test_LDFLAGS = -lcmocka

.PHONY:	tests
tests:
	./test

