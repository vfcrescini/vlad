# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
LFLAGS = -L$(TOPSRCDIR)/lib
TARGETS = epilog

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

all : $(TARGETS)

epilog : epilog.o $(TOPSRCDIR)/lib/*
	$(CC) $(LFLAGS) -o $(@) epilog.o -lparser -lexp -lident -ldatastruct -lfl
ifndef DEBUG
	strip $(@)
endif

epilog.o : epilog.c
	$(CC) $(CFLAGS) -o $(@) -c epilog.c

clean :
	$(RM) $(TARGETS) *.o
