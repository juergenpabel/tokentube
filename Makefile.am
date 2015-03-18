AUTOMAKE_OPTIONS = foreign

SUBDIRS = src/lib src/plugins src/pam src/bin/cli src/bin/pba src/bin/ssod src/bin/sso-greeter-lightdm

include_basedir    = $(prefix)/include
include_librarydir = $(prefix)/include/tokentube
include_plugindir  = $(prefix)/include/tokentube/plugin

include_base_HEADERS = include/tokentube.h
include_library_HEADERS = include/tokentube/defines.h include/tokentube/enums.h include/tokentube/library.h include/tokentube/pba.h include/tokentube/version.h include/tokentube/plugin.h
include_plugin_HEADERS = include/tokentube/plugin/defines.h include/tokentube/plugin/plugin.h

EXTRA_DIST=conf docs

.PHONY:	cmocka
cmocka:
	make -C tests/cmocka
	make -C tests/cmocka tests

.PHONY:	trace
trace:
	make CFLAGS='-DTRACE'

