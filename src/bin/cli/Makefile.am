AUTOMAKE_OPTIONS = foreign

all:
	echo build

install:
	mkdir -p $(prefix)/usr/bin/
	mkdir -p $(prefix)/usr/lib/python3/dist-packages/tokentube_cli
	cp tokentube $(prefix)/usr/bin/
	cp tokentube_cli/*.py $(prefix)/usr/lib/python3/dist-packages/tokentube_cli/

