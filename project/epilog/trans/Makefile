# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libtrans.a
INCLUDES = transdef.h transref.h transdeflist.h transreflist.h

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

libtrans.a : transdef.o transref.o transdeflist.o transreflist.o
	$(AR) -rcs $(@) transdef.o transref.o transdeflist.o transreflist.o

transdef.o : transdef.c transdef.h
	$(CC) $(CFLAGS) -o $(@) -c transdef.c

transref.o : transref.c transref.h
	$(CC) $(CFLAGS) -o $(@) -c transref.c

transdeflist.o : transdeflist.c transdeflist.h transdef.h
	$(CC) $(CFLAGS) -o $(@) -c transdeflist.c

transreflist.o : transreflist.c transreflist.h transref.h
	$(CC) $(CFLAGS) -o $(@) -c transreflist.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
