# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -ansi -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libexp.a
INCLUDES = atom.h expression.h

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

libexp.a : atom.o expression.o
	$(AR) -rcs $(@) atom.o expression.o

atom.o : atom.h atom.c
	$(CC) $(CFLAGS) -o $(@) -c atom.c

expression.o : expression.h expression.c
	$(CC) $(CFLAGS) -o $(@) -c expression.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
