# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libdatastruct.a
INCLUDES = simplelist.h stringlist.h

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

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

libdatastruct.a : simplelist.o stringlist.o
	$(AR) -rcs $(@) simplelist.o stringlist.o

simplelist.o : simplelist.h simplelist.c
	$(CC) $(CFLAGS) -o $(@) -c simplelist.c

stringlist.o : stringlist.h stringlist.c
	$(CC) $(CFLAGS) -o $(@) -c stringlist.c

clean :
	$(RM) $(TARGETS) *.o
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
