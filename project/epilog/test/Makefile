# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -ansi -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = datastruct_test

all : $(TARGETS)

datastruct_test : datastruct_test.c
	$(CC) $(CFLAGS) -o $(@) datastruct_test.c -ldatastruct

clean :
	$(RM) $(TARGETS)
