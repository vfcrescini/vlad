# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libexp.a
INCLUDES = atom.h expression.h varlist.h compatom.h compexp.h

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
	ln -sf ../exp/$$i $(TOPSRCDIR)/include; \
	done
	@for i in $(TARGETS); do \
	ln -sf ../exp/$$i $(TOPSRCDIR)/lib; \
	done

libexp.a : atom.o expression.o varlist.o
	$(AR) -rcs $(@) atom.o expression.o varlist.o

atom.o : atom.h atom.c
	$(CC) $(CFLAGS) -o $(@) -c atom.c

expression.o : expression.h expression.c
	$(CC) $(CFLAGS) -o $(@) -c expression.c

varlist.o : varlist.h varlist.c
	$(CC) $(CFLAGS) -o $(@) -c varlist.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
