CFLAGS = -O2 -Wall
PROGS  =
OBJS   = list.o

all : $(OBJS) $(PROGS)

clean :
	$(RM) $(OBJS) $(PROGS) *.o

list.o : list.c list.h
	$(CC) $(CFLAGS) -c -o $(@) list.c
