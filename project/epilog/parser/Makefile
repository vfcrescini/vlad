# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -ansi -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
TARGETS = libparser.a

all : public

public : $(TARGETS)
	@mkdir -p $(TOPSRCDIR)/include
	@mkdir -p $(TOPSRCDIR)/lib
	@for i in $(INCLUDES); do \
	ln -sf ../parser/$$i $(TOPSRCDIR)/include; \
	done
	@for i in $(TARGETS); do \
	ln -sf ../parser/$$i $(TOPSRCDIR)/lib; \
	done

libparser.a : parser.o scanner.o 
	$(AR) -rcs $(@) parser.o scanner.o

scanner.o : y.tab.h lex.yy.c
	$(CC) $(CFLAGS) -o $(@) -c lex.yy.c

parser.o : y.tab.h y.tab.c
	$(CC) $(CFLAGS) -o $(@) -c y.tab.c

lex.yy.c : scanner.l
	flex scanner.l

y.tab.c : parser.y
	bison -y parser.y

y.tab.h : parser.y
	bison -yd parser.y

clean :
	$(RM) $(TARGETS) *.o lex.yy.c y.tab.c y.tab.h
	@for i in $(TARGETS); do \
	$(RM) $(TOPSRCDIR)/lib/$$i; \
	done
