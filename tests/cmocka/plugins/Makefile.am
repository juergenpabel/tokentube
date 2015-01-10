AUTOMAKE_OPTIONS = foreign

SUBDIRS = 00_compile 01_init

.PHONY:	tests
tests:
	make -C 00_compile	tests
	make -C 01_init		tests

