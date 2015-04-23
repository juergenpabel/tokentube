
EXTRA_DIST = $(MANS) $(XMLS)

man_MANS = tokentube.1 tokentube.conf.5

XMLS = tokentube.1.xml tokentube.conf.5.xml


%.1: ../xml/%.1.xml
	$(XMLLINT) --nonet --xinclude --postvalid --noout ../xml/$<
	$(XSLTPROC) -o $@ --path $(srcdir) --xinclude --nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl ../xml/$<

%.5: ../xml/%.5.xml
	$(XMLLINT) --nonet --xinclude --postvalid --noout ../xml/$<
	$(XSLTPROC) -o $@ --path $(srcdir) --xinclude --nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl ../xml/$<

