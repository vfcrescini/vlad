
SUBDIRS = src tests m4

EXTRA_DIST = setup.bat README.Windows

dist-hook:
	mkdir $(distdir)/doc
	cp -pd doc/lparse.ps $(distdir)/doc
	mkdir $(distdir)/examples
	mkdir $(distdir)/examples/puzzle
	cp -rp examples/* $(distdir)/examples
	mkdir $(distdir)/lib
	cp -p lib/* $(distdir)/lib 

