# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -ansi -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libdatastruct.a
INCLUDES = index.h indexedlist.h simplelist.h

all : public

public : $(TARGETS)
	@mkdir -p $(TOPSRCDIR)/include
	@mkdir -p $(TOPSRCDIR)/lib
	@for i in $(INCLUDES); do \
	ln -sf ../datastruct/$$i $(TOPSRCDIR)/include; \
	done
	@for i in $(TARGETS); do \
	ln -sf ../datastruct/$$i $(TOPSRCDIR)/lib; \
	done

libdatastruct.a : index.o indexedlist.o simplelist.o
	$(AR) -rcs $(@) index.o indexedlist.o simplelist.o

index.o : index.h index.c
	$(CC) $(CFLAGS) -o $(@) -c index.c
	
indexedlist.o : indexedlist.h indexedlist.c index.h
	$(CC) $(CFLAGS) -o $(@) -c indexedlist.c

simplelist.o : simplelist.h simplelist.c
	$(CC) $(CFLAGS) -o $(@) -c simplelist.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
