CFLAGS	= -O3
X11DIR	= /usr/X11R6/lib
X11MOD	= X11 

PROGS	=		\
	stest		\
	$(NULL)	

all : $(PROGS) 

stest : structure.o stack.o queue.o circular.o test.cpp
	$(CC) $(CFLAGS) -o $@ test.cpp stack.o queue.o circular.o structure.o 

stack.o : stack.cpp stack.h 
	$(CC) $(CFLAGS) -c stack.cpp

queue.o : queue.cpp queue.h 
	$(CC) $(CFLAGS) -c queue.cpp

circular.o : circular.cpp circular.h 
	$(CC) $(CFLAGS) -c circular.cpp 

structure.o : structure.cpp structure.h
	$(CC) $(CFLAGS) -c structure.cpp

clean :
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS) 
