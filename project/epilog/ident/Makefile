# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libident.a
INCLUDES = ident.h identlist.h

all : public

public : $(TARGETS)
	@mkdir -p $(TOPSRCDIR)/include
	@mkdir -p $(TOPSRCDIR)/lib
	@for i in $(INCLUDES); do \
	ln -sf ../ident/$$i $(TOPSRCDIR)/include; \
	done
	@for i in $(TARGETS); do \
	ln -sf ../ident/$$i $(TOPSRCDIR)/lib; \
	done

libident.a : ident.o identlist.o
	$(AR) -rcs $(@) ident.o identlist.o

ident.o : ident.h ident.c
	$(CC) $(CFLAGS) -o $(@) -c ident.c

identlist.o : ident.h identlist.h identlist.c
	$(CC) $(CFLAGS) -o $(@) -c identlist.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
