CFLAGS = -O2 -Wall
PROGS  = test
OBJS   = list.o

all : $(OBJS) $(PROGS)

clean :
	$(RM) $(OBJS) $(PROGS) *.o

list.o : list.c list.h
	$(CC) $(CFLAGS) -c -o $(@) list.c

test : test.c $(OBJS)
	$(CC) $(CFLAGS) -o test test.c $(OBJS)
