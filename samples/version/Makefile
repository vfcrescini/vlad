# Makefile
# Vino Crescini
# 1 Jan 2002

CFLAGS=-O3 -Wall
PROGS=version

all : $(PROGS)

version : version.c
	$(CC) $(CFLAGS) version.c -o $(@)

strip : $(PROGS)
	strip --strip-all $(PROGS)

clean :
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS)
