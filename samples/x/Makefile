CFLAGS	= -O3
X11DIR	= /usr/X11R6/lib
X11MOD	= X11 

PROGS	=		\
	xtest		\
	$(NULL)	
	

all : $(PROGS) 

xtest : xtest.c
	$(CC) $(CFLAGS) -o $@ -L$(X11DIR) -l$(X11MOD) xtest.c  

clean :
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS) 
