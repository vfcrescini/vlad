CFLAGS	= -O3 

PROGS	=		\
	gtkfun		\
	$(NULL)	
	

all : $(PROGS) 

gtkfun : gtkfun.c
	$(CC) $(CFLAGS) -o $@ gtkfun.c `gtk-config --cflags --libs`

clean :
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS) 
