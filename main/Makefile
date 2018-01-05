# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
LDFLAGS = -L$(TOPSRCDIR)/lib
TARGETS = vlad

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

all : $(TARGETS)

vlad : vlad.o $(TOPSRCDIR)/lib/*
	$(CC) $(LDFLAGS) -o $(@) vlad.o -lparser -lfl
	@for i in $(TARGETS); do \
	ln -sf main/$$i $(TOPSRCDIR); \
	done
ifndef DEBUG
	strip $(@)
endif

vlad.o : vlad.c
	$(CC) $(CFLAGS) -o $(@) -c vlad.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/$$i; \
	done
