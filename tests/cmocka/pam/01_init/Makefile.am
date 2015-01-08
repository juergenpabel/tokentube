AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = test

test_SOURCES = test.c
test_CFLAGS  = -I../../../../include
test_LDFLAGS = -Llib -Llib/security -lpam_tokentube -ltokentube -lcmocka

.PHONY:	tests
tests:
	LD_PRELOAD=lib/libpam.so LD_LIBRARY_PATH=lib:lib/security ./test

