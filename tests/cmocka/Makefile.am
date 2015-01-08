AUTOMAKE_OPTIONS = foreign

SUBDIRS = library plugins pam

override CFLAGS =
override CPPFLAGS =
override LDFLAGS =

.PHONY:	tests
tests:
	make -C library	tests
	make -C plugins	tests
	make -C pam	tests

install:
	echo "cmocka: make install (ignoring...)"

