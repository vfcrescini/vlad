# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -ansi -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libsymtab.a
INCLUDES = symtab.h

all : public

public : $(TARGETS)
	@mkdir -p $(TOPSRCDIR)/include
	@mkdir -p $(TOPSRCDIR)/lib
	@for i in $(INCLUDES); do \
	ln -sf ../symtab/$$i $(TOPSRCDIR)/include; \
	done
	@for i in $(TARGETS); do \
	ln -sf ../symtab/$$i $(TOPSRCDIR)/lib; \
	done

libsymtab.a : symtab.o
	$(AR) -rcs $(@) symtab.o

symtab.o : symtab.h symtab.c
	$(CC) $(CFLAGS) -o $(@) -c symtab.c
	
clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
