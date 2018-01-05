# Vino Crescini  <jcrescin@cit.uws.edu.au>

.PHONY :                  \
  copy-includes           \
  copy-static-libs        \
  copy-shared-libs        \
  copy-bins               \
  install-includes        \
  install-static-libs     \
  install-shared-libs     \
  install-bins            \
  uninstall-includes      \
  uninstall-static-libs   \
  uninstall-shared-libs   \
  uninstall-bins          \
  all                     \
  clean                   \
  distclean               \
  $(NULL)

all :                     \
  copy-includes           \
  copy-static-libs        \
  copy-shared-libs        \
  copy-bins               \
  $(NULL)

install :                 \
  all                     \
  install-includes        \
  install-static-libs     \
  install-shared-libs     \
  install-bins            \
  $(NULL)

uninstall :               \
  uninstall-includes      \
  uninstall-static-libs   \
  uninstall-shared-libs   \
  uninstall-bins          \
  $(NULL)

copy-includes : $(INCLUDES)
ifdef INCLUDES
	@mkdir -p $(DISTDIR)/include/vlad
	@for i in $(INCLUDES); do                                              \
	(cd $(DISTDIR)/include/vlad && $(LN_S) -f ../../../$(CURDIR)/$$i $$i); \
	done
endif

copy-static-libs : $(STATIC_LIBS)
ifdef STATIC_LIBS
	@mkdir -p $(DISTDIR)/lib
	@for i in $(STATIC_LIBS); do                                           \
	(cd $(DISTDIR)/lib && $(LN_S) -f ../../$(CURDIR)/$$i $$i);             \
	done
endif

ifdef BUILDSHARED
copy-shared-libs : $(SHARED_LIBS)
  ifdef SHARED_LIBS
	@mkdir -p $(DISTDIR)/lib
	@for i in $(SHARED_LIBS); do                                           \
	(cd $(DISTDIR)/lib && $(LN_S) -f ../../$(CURDIR)/$$i $$i);             \
        (cd $(DISTDIR)/lib && $(LN_S) -f $$i `echo $$i | cut -d. -f1,2`);      \
	(cd $(DISTDIR)/lib && $(LN_S) -f $$i `echo $$i | cut -d. -f1,2,3`);    \
	done
  endif
endif

copy-bins : $(BINS)
ifdef BINS
	@mkdir -p $(DISTDIR)/bin
	@for i in $(BINS); do                                                  \
	(cd $(DISTDIR)/bin && $(LN_S) -f ../../$(CURDIR)/$$i $$i);             \
	done
endif

install-includes : $(INCLUDES)
ifdef INCLUDES
	@mkdir -p $(includedir)/vlad
	@for i in $(INCLUDES); do                                              \
	cp -f $$i $(includedir)/vlad/$$i;                                      \
	done
endif

install-static-libs : $(STATIC_LIBS)
ifdef STATIC_LIBS
	@mkdir -p $(libdir)
	@for i in $(STATIC_LIBS); do                                           \
	cp -f $$i $(libdir)/$$i;                                               \
	done
endif

ifdef BUILDSHARED
install-shared-libs : $(SHARED_LIBS)
  ifdef SHARED_LIBS
	@mkdir -p $(libdir)
	@for i in $(SHARED_LIBS); do                                           \
	cp -f $$i $(libdir)/$$i;                                               \
        (cd $(libdir) && $(LN_S) -f $$i `echo $$i | cut -d. -f1,2`);           \
	(cd $(libdir) && $(LN_S) -f $$i `echo $$i | cut -d. -f1,2,3`);         \
	done
  endif
endif

install-bins : $(BINS)
ifdef BINS
	@mkdir -p $(bindir)
	@for i in $(BINS); do                                                  \
	cp -f $$i $(bindir)/$$i;                                               \
	done
endif

uninstall-includes : $(INCLUDES)
ifdef INCLUDES
	@for i in $(INCLUDES); do                                              \
	$(RM) $(includedir)/vlad/$$i;                                          \
	done
	@rm -fr $(includedir)/vlad
endif

uninstall-static-libs : $(STATIC_LIBS)
ifdef STATIC_LIBS
	@for i in $(STATIC_LIBS); do                                           \
	$(RM) $(libdir)/$$i;                                                   \
	done
endif

ifdef BUILDSHARED
uninstall-shared-libs : $(SHARED_LIBS)
  ifdef SHARED_LIBS
	@for i in $(SHARED_LIBS); do                                           \
	$(RM) $(libdir)/$$i;                                                   \
        $(RM) $(libdir)/`echo $$i | cut -d. -f1,2`;                            \
	$(RM) $(libdir)/`echo $$i | cut -d. -f1,2,3`;                          \
	done
  endif
endif

uninstall-bins : $(BINS)
ifdef BINS
	@for i in $(BINS); do                                                  \
	$(RM) $(bindir)/$$i;                                                   \
	done
endif

clean :
	$(RM) $(OBJECTS) $(TEMP)

distclean : clean
	$(RM) $(STATIC_LIBS) $(SHARED_LIBS) $(BINS) Makefile
ifdef INCLUDES
	@for i in $(INCLUDES); do                                              \
	$(RM) $(DISTDIR)/include/vlad/$$i;                                     \
	done
endif
ifdef STATIC_LIBS
	@for i in $(STATIC_LIBS); do                                           \
	$(RM) $(DISTDIR)/lib/$$i;                                              \
	done
endif
ifdef SHARED_LIBS
	@for i in $(SHARED_LIBS); do                                           \
	$(RM) $(DISTDIR)/lib/$$i;                                              \
	done
endif
ifdef BINS
	@for i in $(BINS); do                                                  \
	$(RM) $(DISTDIR)/bin/$$i;                                              \
	done
endif
