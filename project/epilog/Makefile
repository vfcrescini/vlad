# Vino Crescini  <jcrescin@cit.uws.edu.au>

DIRS = datastruct ident exp parser main
DEBUG_DIRS = test

all :
	@set -e && for i in $(DIRS); do \
	$(MAKE) -C $$i all; \
	done
ifdef DEBUG
	@set -e && for i in $(DEBUG_DIRS); do \
	$(MAKE) -C $$i all; \
	done
endif

clean :
	@set -e && for i in $(DIRS) $(DEBUG_DIRS); do \
	$(MAKE) -C $$i clean; \
	done
	$(RM) -r include lib
