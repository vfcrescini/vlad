# Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>

.PHONY : \
  copy-includes \
  copy-static-libs \
  copy-shared-libs \
  copy-bins \
  install-includes \
  install-static-libs \
  install-shared-libs \
  install-bins \
  uninstall-includes \
  uninstall-static-libs \
  uninstall-shared-libs \
  uninstall-bins \
  all \
  clean \
  distclean \
  $(NULL)

define exec
  $(1)

endef

all : \
  copy-includes \
  copy-static-libs \
  copy-shared-libs \
  copy-bins \
  $(NULL)

install : \
  all \
  install-includes \
  install-static-libs \
  install-shared-libs \
  install-bins \
  $(NULL)

uninstall : \
  uninstall-includes \
  uninstall-static-libs \
  uninstall-shared-libs \
  uninstall-bins \
  $(NULL)

copy-includes : $(INCLUDES)
	mkdir -p $(DISTDIR)/include/vlad
	$(foreach i,$(INCLUDES),$(call exec,$(LN_S) -f ../../../$(CURDIR)/$(i) $(DISTDIR)/include/vlad/$(i)))

copy-static-libs : $(STATIC_LIBS)
	mkdir -p $(DISTDIR)/lib
	$(foreach i,$(STATIC_LIBS),$(call exec,$(LN_S) -f ../../$(CURDIR)/$(i) $(DISTDIR)/lib/$(i)))

ifdef BUILDSHARED
copy-shared-libs : $(SHARED_LIBS)
	mkdir -p $(DISTDIR)/lib
	$(foreach i,$(SHARED_LIBS),$(call exec,$(LN_S) -f ../../$(CURDIR)/$(i) $(DISTDIR)/lib/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(LN_S) -f $(i) $(DISTDIR)/lib/$(shell echo $(i) | cut -d. -f1,2,3)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(LN_S) -f $(i) $(DISTDIR)/lib/$(shell echo $(i) | cut -d. -f1,2)))
endif

copy-bins : $(BINS)
	mkdir -p $(DISTDIR)/bin
	$(foreach i,$(BINS),$(call exec,$(LN_S) -f ../../$(CURDIR)/$(i) $(DISTDIR)/bin/$(i)))

install-includes : $(INCLUDES)
	mkdir -p $(includedir)/vlad
	$(foreach i,$(INCLUDES),$(call exec,cp -f $(i) $(includedir)/vlad/$(i)))

install-static-libs : $(STATIC_LIBS)
	mkdir -p $(libdir)
	$(foreach i,$(STATIC_LIBS),$(call exec,cp -f $(i) $(libdir)/$(i)))

ifdef BUILDSHARED
install-shared-libs : $(SHARED_LIBS)
	mkdir -p $(libdir)
	$(foreach i,$(SHARED_LIBS),$(call exec,cp -f $(i) $(libdir)/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(LN_S) -f $(i) $(libdir)/$(shell echo $(i) | cut -d. -f1,2,3)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(LN_S) -f $(i) $(libdir)/$(shell echo $(i) | cut -d. -f1,2)))
endif

install-bins : $(BINS)
	mkdir -p $(bindir)
	$(foreach i,$(BINS),$(call exec,cp -f $(i) $(bindir)/$(i)))

uninstall-includes :
	$(foreach i,$(INCLUDES),$(call exec,$(RM) $(includedir)/vlad/$(i)))
	$(RM) -r $(includedir)/vlad

uninstall-static-libs :
	$(foreach i,$(STATIC_LIBS),$(call exec,$(RM) $(libdir)/$(i)))

ifdef BUILDSHARED
uninstall-shared-libs :
	$(foreach i,$(SHARED_LIBS),$(call exec,$(RM) $(libdir)/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(RM) $(libdir)/$(shell echo $(i) | cut -d. -f1,2,3)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(RM) $(libdir)/$(shell echo $(i) | cut -d. -f1,2)))
endif

uninstall-bins :
	$(foreach i,$(BINS),$(call exec,$(RM) $(bindir)/$(i)))

clean :
	$(foreach i,$(TEMPS1) $(OBJECTS),$(call exec,$(RM) $(i)))

distclean : clean
	$(foreach i,$(INCLUDES),$(call exec,$(RM) $(DISTDIR)/include/vlad/$(i)))
	$(foreach i,$(STATIC_LIBS),$(call exec,$(RM) $(DISTDIR)/lib/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(RM) $(DISTDIR)/lib/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(RM) $(DISTDIR)/lib/$(shell echo $(i) | cut -d. -f1,2,3)))
	$(foreach i,$(SHARED_LIBS),$(call exec,$(RM) $(DISTDIR)/lib/$(shell echo $(i) | cut -d. -f1,2)))
	$(foreach i,$(BINS),$(call exec,$(RM) $(DISTDIR)/bin/$(i)))
	$(RM) -r $(DISTDIR)/include/vlad
	$(RM) $(TEMPS2) $(STATIC_LIBS) $(SHARED_LIBS) $(BINS) Makefile
