CFLAGS = -Wall -ansi -O2
PROGS  = zaphodd
OBJS   = list.o

all : $(PROGS) $(OBJS)

clean :
	$(RM) $(PROGS) $(OBJS)

zaphodd : zaphodd.c
	$(CC) $(CFLAGS) -o $(@) zaphodd.c

list.o : list.c list.h
	$(CC) $(CFLAGS) -o $(@) -c list.c
