# Vino Crescini  <jcrescin@cit.uws.edu.au>

.PHONY : includes static-libs shared-libs bins all clean distclean

all : includes static-libs shared-libs bins

includes : $(INCLUDES)
ifdef INCLUDES
	@mkdir -p $(DISTDIR)/include/vlad
	@for i in $(INCLUDES); do \
	(cd $(DISTDIR)/include/vlad && $(LN_S) -f ../../../$(CURDIR)/$$i $$i); \
	done
endif

static-libs : $(STATIC_LIBS)
ifdef STATIC_LIBS
	@mkdir -p $(DISTDIR)/lib
	@for i in $(STATIC_LIBS); do \
	(cd $(DISTDIR)/lib && $(LN_S) -f ../../$(CURDIR)/$$i $$i); \
	done
endif

ifdef BUILDSHARED
shared-libs : $(SHARED_LIBS)
  ifdef SHARED_LIBS
	@mkdir -p $(DISTDIR)/lib
	@for i in $(SHARED_LIBS); do \
	(cd $(DISTDIR)/lib && $(LN_S) -f ../../$(CURDIR)/$$i $$i); \
        (cd $(DISTDIR)/lib && $(LN_S) -f $$i `echo $$i | cut -d. -f1,2`); \
	(cd $(DISTDIR)/lib && $(LN_S) -f $$i `echo $$i | cut -d. -f1,2,3`); \
	done
  endif
endif

bins : $(BINS)
ifdef BINS
	@mkdir -p $(DISTDIR)/bin
	@for i in $(BINS); do \
	(cd $(DISTDIR)/bin && $(LN_S) -f ../../$(CURDIR)/$$i $$i); \
	done
endif

clean :
	$(RM) $(OBJECTS) $(TEMP)

distclean : clean
	$(RM) $(STATIC_LIBS) $(SHARED_LIBS) $(BINS) Makefile
ifdef INCLUDES
	@for i in $(INCLUDES); do \
	$(RM) $(DISTDIR)/include/vlad/$$i; \
	done
endif
ifdef STATIC_LIBS
	@for i in $(STATIC_LIBS); do \
	$(RM) $(DISTDIR)/lib/$$i; \
	done
endif
ifdef SHARED_LIBS
	@for i in $(SHARED_LIBS); do \
	$(RM) $(DISTDIR)/lib/$$i; \
	done
endif
ifdef BINS
	@for i in $(BINS); do \
	$(RM) $(DISTDIR)/bin/$$i; \
	done
endif
