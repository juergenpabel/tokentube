AUTOMAKE_OPTIONS = foreign

SUBDIRS = 00_compile 01_init 02_config 03_testvectors 04_user

.PHONY:	tests
tests:
	make -C 00_compile	tests
	make -C 01_init		tests
	make -C 02_config	tests
	make -C 03_testvectors	tests
	make -C 04_user		tests

