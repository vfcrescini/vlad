# Vino Crescini  <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -O2 -ansi -I${TOPSRCDIR}/include -L${TOPSRCDIR}/lib
WFLAGS = -Wall
TARGETS = main

all : $(TARGETS)

main : parser.o scanner.o
	$(CC) $(CFLAGS) $(WFLAGS) -o$(@) parser.o scanner.o -ll -lsymtab -ldatastruct

scanner.o : y.tab.h lex.yy.c
	$(CC) $(CFLAGS) -o$(@) -c lex.yy.c

parser.o : y.tab.h y.tab.c
	$(CC) $(CFLAGS) -o$(@) -c y.tab.c

lex.yy.c : scanner.l
	$(LEX) scanner.l

y.tab.c : parser.y
	$(YACC) parser.y

y.tab.h : parser.y
	$(YACC) -d parser.y

clean :
	$(RM) $(TARGETS) *.o lex.yy.c y.tab.c y.tab.h
