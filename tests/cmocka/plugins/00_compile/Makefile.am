AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = test

test_SOURCES = test.c
test_CFLAGS  = -I../../../../include
test_LDFLAGS = -lcmocka -ldl -Wl,--export-dynamic

.PHONY:	tests
tests:
	./test

