AUTOMAKE_OPTIONS = foreign

SUBDIRS = libpam 00_compile 01_init 02_sm_authenticate 03_sm_chauthtok

.PHONY:	tests
tests:
	make -C 00_compile		tests
	make -C 01_init			tests
	make -C 02_sm_authenticate 	tests
	make -C 03_sm_chauthtok		tests

