# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libexp.a
INCLUDES = expression.h name.h gnd_atom.h gnd_exp.h comp_atom.h comp_exp.h

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

libexp.a : name.o gnd_atom.o gnd_exp.o comp_atom.o comp_exp.o
	$(AR) -rcs $(@) name.o gnd_atom.o gnd_exp.o comp_atom.o comp_exp.o

name.o : name.h name.c
	$(CC) $(CFLAGS) -o $(@) -c name.c

gnd_atom.o : gnd_atom.h gnd_atom.c
	$(CC) $(CFLAGS) -o $(@) -c gnd_atom.c

gnd_exp.o : gnd_exp.h gnd_exp.c
	$(CC) $(CFLAGS) -o $(@) -c gnd_exp.c

comp_atom.o : comp_atom.h comp_atom.c
	$(CC) $(CFLAGS) -o $(@) -c comp_atom.c

comp_exp.o : comp_exp.h comp_exp.c
	$(CC) $(CFLAGS) -o $(@) -c comp_exp.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
