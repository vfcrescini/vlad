# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -ansi -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
LFLAGS = -L$(TOPSRCDIR)/lib
TARGETS = epilog

all : $(TARGETS)

epilog : epilog.o $(TOPSRCDIR)/lib/*
	$(CC) $(LFLAGS) -o $(@) epilog.o -lparser -lident -ldatastruct -lfl
ifndef DEBUG
	strip $(@)
endif

epilog.o : epilog.c
	$(CC) $(CFLAGS) -o $(@) -c epilog.c

clean :
	$(RM) $(TARGETS) *.o
