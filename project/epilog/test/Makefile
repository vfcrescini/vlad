# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
LFLAGS = -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = datastruct_test

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

all : $(TARGETS)

datastruct_test : datastruct_test.o
	$(CC) $(LFLAGS) -o $(@) datastruct_test.o -ldatastruct

datastruct_test.o : datastruct_test.c
	$(CC) $(CFLAGS) -o $(@) -c datastruct_test.c

clean :
	$(RM) $(TARGETS)
