CFLAGS = -Wall -ansi -O2
PROGS  = zaphodd
OBJS   = list.o handler.o

all : $(OBJS) $(PROGS)

clean :
	$(RM) $(PROGS) $(OBJS)

zaphodd : zaphodd.c $(OBJS)
	$(CC) $(CFLAGS) -o $(@) $(OBJS) zaphodd.c

list.o : list.c list.h
	$(CC) $(CFLAGS) -o $(@) -c list.c

handler.o : handler.c handler.h
	$(CC) $(CFLAGS) -o $(@) -c handler.c
