# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
LDFLAGS = -L$(TOPSRCDIR)/lib
TARGETS = epilog

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

all : $(TARGETS)

epilog : epilog.o $(TOPSRCDIR)/lib/*
	$(CC) $(LDFLAGS) -o $(@) epilog.o -lparser -ltrans -lexp -lident -ldatastruct -lfl
	@for i in $(TARGETS); do \
	ln -sf main/$$i $(TOPSRCDIR); \
	done
ifndef DEBUG
	strip $(@)
endif

epilog.o : epilog.c
	$(CC) $(CFLAGS) -o $(@) -c epilog.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/$$i; \
	done
