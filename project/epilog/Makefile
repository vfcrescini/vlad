# Vino Crescini  <jcrescin@cit.uws.edu.au>

DIRS = datastruct ident parser main
ifdef DEBUG
DIRS += test
endif

all :
	@set -e && for i in $(DIRS); do \
	$(MAKE) -C $$i all; \
	done

clean :
	@for i in $(DIRS); do \
	$(MAKE) -C $$i clean; \
	done
	$(RM) -r include lib
