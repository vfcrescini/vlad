CFLAGS = -O3 -Wall
PROGS  = rcmdd
STRIP  = strip

all : strip-all clean

strip-all : $(PROGS)
	$(STRIP) $(PROGS)

rcmdd : rcmdd.c
	$(CC) $(CFLAGS) -o $(@) rcmdd.c

clean :
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS)
