# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libtrans.a
INCLUDES =

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

all : public

public : $(TARGETS)
	@mkdir -p $(TOPSRCDIR)/include
	@mkdir -p $(TOPSRCDIR)/lib
	@for i in $(INCLUDES); do \
	ln -sf ../trans/$$i $(TOPSRCDIR)/include; \
	done
	@for i in $(TARGETS); do \
	ln -sf ../trans/$$i $(TOPSRCDIR)/lib; \
	done

libtrans.a :
	$(AR) -rcs $(@)

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
