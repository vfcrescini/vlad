# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -ansi -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libkb.a
INCLUDES = atom_type.h gnd_atom.h gnd_atomlist.h

all : public

public : $(TARGETS)
	@mkdir -p $(TOPSRCDIR)/include
	@mkdir -p $(TOPSRCDIR)/lib
	@for i in $(INCLUDES); do \
	ln -sf ../kb/$$i $(TOPSRCDIR)/include; \
	done
	@for i in $(TARGETS); do \
	ln -sf ../kb/$$i $(TOPSRCDIR)/lib; \
	done

libkb.a : gnd_atom.o gnd_atomlist.o
	$(AR) -rcs $(@) gnd_atom.o gnd_atomlist.o

gnd_atom.o : gnd_atom.c gnd_atom.h
	$(CC) $(CFLAGS) -o $(@) -c gnd_atom.c

gnd_atomlist.o : gnd_atomlist.c gnd_atomlist.h gnd_atom.h
	$(CC) $(CFLAGS) -o $(@) -c gnd_atomlist.c
	
clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
