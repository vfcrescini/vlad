# Vino Crescini  <jcrescin@cit.uws.edu.au>

all : copy-includes copy-libs copy-bins

copy-includes : $(INCLUDES)
	@mkdir -p $(DISTDIR)/include/vlad
	@for i in $(INCLUDES); do \
	(cd $(DISTDIR)/include/vlad && $(LN_S) -f ../../../$(CURDIR)/$$i $$i); \
	done

copy-libs : $(LIBS)
	@mkdir -p $(DISTDIR)/lib/vlad
	@for i in $(LIBS); do \
	(cd $(DISTDIR)/lib/vlad && $(LN_S) -f ../../../$(CURDIR)/$$i $$i); \
	done

copy-bins : $(BINS)
	@mkdir -p $(DISTDIR)/bin
	@for i in $(BINS); do \
	(cd $(DISTDIR)/bin && $(LN_S) -f ../../$(CURDIR)/$$i $$i); \
	done

clean :
	$(RM) $(OBJECTS) $(TEMPS)

distclean : clean
	$(RM) $(LIBS) $(BINS) Makefile
	@for i in $(INCLUDES); do \
	$(RM) $(DISTDIR)/include/vlad/$$i; \
	done
	@for i in $(LIBS); do \
	$(RM) $(DISTDIR)/lib/vlad/$$i; \
	done
	@for i in $(BINS); do \
	$(RM) $(DISTDIR)/bin/$$i; \
	done
