# Vino Crescini  <jcrescin@cit.uws.edu.au>

DIRS = datastruct symtab kb parser main

all :
	@for i in $(DIRS); do \
	$(MAKE) -C $$i all; \
	done

clean :
	@for i in $(DIRS); do \
	$(MAKE) -C $$i clean; \
	done
	$(RM) -r include lib
