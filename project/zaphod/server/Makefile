CFLAGS = -Wall -ansi -O2
PROGS  = zaphodd

all : $(PROGS)

clean :
	$(RM) $(PROGS) *.o

zaphodd : zaphodd.c
	$(CC) $(CFLAGS) -o $(@) zaphodd.c
