# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libexp.a
INCLUDES = name.h atom.h expression.h

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

libexp.a : name.o atom.o expression.o
	$(AR) -rcs $(@) name.o atom.o expression.o

name.o : name.h name.c
	$(CC) $(CFLAGS) -o $(@) -c name.c

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
