
check_SCRIPTS = test_all

TESTS = test_all

EXTRA_DIST = test_all.pl 

dist-hook:
	mkdir $(distdir)/results
	cp -p results/* $(distdir)/results
	cp -p *.lp $(distdir)

test_all : test_all.pl
	rm -f test_all
	echo "#! " $(PERL) > test_all
	cat test_all.pl >> test_all
	chmod 755 test_all

