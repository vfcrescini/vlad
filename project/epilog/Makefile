# Vino Crescini  <jcrescin@cit.uws.edu.au>

DIRS = datastruct ident exp trans parser main

all :
	@set -e && for i in $(DIRS); do \
	$(MAKE) -C $$i all; \
	done

clean :
	@set -e && for i in $(DIRS); do \
	$(MAKE) -C $$i clean; \
	done
	$(RM) -r include lib
