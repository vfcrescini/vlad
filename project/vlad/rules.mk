# Vino Crescini  <jcrescin@cit.uws.edu.au>

all : copy-includes copy-libs copy-bins

copy-includes : $(INCLUDES)
	@mkdir -p $(DISTDIR)/include/vlad
	@if [ "x$(INCLUDES)" != "x" ]; then \
	for i in $(INCLUDES); do \
	(cd $(DISTDIR)/include/vlad && $(LN_S) -f ../../../$(CURDIR)/$$i $$i); \
	done; \
	fi

copy-libs : $(LIBS)
	@mkdir -p $(DISTDIR)/lib/vlad
	@if [ "x$(LIBS)" != "x" ]; then \
	for i in $(LIBS); do \
	(cd $(DISTDIR)/lib/vlad && $(LN_S) -f ../../../$(CURDIR)/$$i $$i); \
	done; \
	fi

copy-bins : $(BINS)
	@mkdir -p $(DISTDIR)/bin
	@if [ "x$(BINS)" != "x" ]; then \
	for i in $(BINS); do \
	(cd $(DISTDIR)/bin && $(LN_S) -f ../../$(CURDIR)/$$i $$i); \
	done; \
	fi

clean :
	$(RM) $(OBJECTS) $(TEMPS)

distclean : clean
	$(RM) $(LIBS) $(BINS) Makefile
	@if [ "x$(INCLUDES)" != "x" ]; then \
	for i in $(INCLUDES); do \
	$(RM) $(DISTDIR)/include/vlad/$$i; \
	done; \
	fi
	@if [ "x$(LIBS)" != "x" ]; then \
	for i in $(LIBS); do \
	$(RM) $(DISTDIR)/lib/vlad/$$i; \
	done; \
	fi
	@if [ "x$(BINS)" != "x" ]; then \
	for i in $(BINS); do \
	$(RM) $(DISTDIR)/bin/$$i; \
	done; \
	fi
