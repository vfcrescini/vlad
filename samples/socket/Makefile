CFLAGS = 		\
	-O3		\
	-Wall		\
	$(NULL)

PROGS =			\
	client		\
	server		\
	$(NULL)

all : $(PROGS)

server : server.c
	$(CC) $(CFLAGS) -o $(@) $(@).c

client : client.c
	$(CC) $(CFLAGS) -o $(@) $(@).c

strip : $(PROGS)
	strip $(PROGS)
clean :
	rm -f *.o

distclean : clean
	rm -f $(PROGS)
