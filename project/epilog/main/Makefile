# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -ansi -I $(TOPSRCDIR)/include -L $(TOPSRCDIR)/lib
TARGETS = epilog

all : $(TARGETS)

epilog : epilog.c
	$(CC) $(CFLAGS) epilog.c -o $(@) -lparser -lsymtab -ldatastruct -ll
	strip $(@)

clean :
	$(RM) $(TARGETS)
