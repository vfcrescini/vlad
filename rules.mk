# Vino Crescini  <jcrescin@cit.uws.edu.au>

all : copy-includes copy-libs copy-bins

copy-includes : $(INCLUDES)
ifdef INCLUDES
	@mkdir -p $(DISTDIR)/include/vlad
	@for i in $(INCLUDES); do \
	(cd $(DISTDIR)/include/vlad && $(LN_S) -f ../../../$(CURDIR)/$$i $$i); \
	done
endif

copy-libs : $(LIBS)
ifdef LIBS
	@mkdir -p $(DISTDIR)/lib/vlad
	@for i in $(LIBS); do \
	(cd $(DISTDIR)/lib/vlad && $(LN_S) -f ../../../$(CURDIR)/$$i $$i); \
	done
endif

copy-bins : $(BINS)
ifdef BINS
	@mkdir -p $(DISTDIR)/bin
	@for i in $(BINS); do \
	(cd $(DISTDIR)/bin && $(LN_S) -f ../../$(CURDIR)/$$i $$i); \
	done
endif

clean :
	$(RM) $(OBJECTS) $(TEMPS)

distclean : clean
	$(RM) $(LIBS) $(BINS) Makefile
ifdef INCLUDES
	@for i in $(INCLUDES); do \
	$(RM) $(DISTDIR)/include/vlad/$$i; \
	done
endif
ifdef LIBS
	@for i in $(LIBS); do \
	$(RM) $(DISTDIR)/lib/vlad/$$i; \
	done
endif
ifdef BINS
	@for i in $(BINS); do \
	$(RM) $(DISTDIR)/bin/$$i; \
	done
endif
