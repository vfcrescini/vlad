# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
LFLAGS = -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = datastruct_test

all : $(TARGETS)

datastruct_test : datastruct_test.c
	$(CC) $(LFLAGS) -o $(@) datastruct_test.c -ldatastruct

clean :
	$(RM) $(TARGETS)
