AUTOMAKE_OPTIONS = foreign

SUBDIRS = 00_compile 01_init 02_helpdesk

.PHONY:	tests
tests:
	make -C 00_compile	tests
	make -C 01_init		tests
	make -C 02_helpdesk	tests

