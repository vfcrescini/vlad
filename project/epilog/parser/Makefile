# Vino Crescini
# 2002 May 30

TOPSRCDIR = ..
CFLAGS = -Wall -O2 
DIRFLAGS = -I${TOPSRCDIR}/include -L${TOPSRCDIR}/lib
TARGETS = main

all : $(TARGETS)

main : parser.o scanner.o
	$(CC) $(CFLAGS) $(DIRFLAGS) -o$(@) parser.o scanner.o -ll -lsymtab -ldatastruct

scanner.o : y.tab.h lex.yy.c
	$(CC) $(DIRFLAGS) -o$(@) -c lex.yy.c

parser.o : y.tab.h y.tab.c
	$(CC) $(DIRFLAGS) -o$(@) -c y.tab.c

lex.yy.c : scanner.l
	$(LEX) scanner.l

y.tab.c : parser.y
	$(YACC) parser.y

y.tab.h : parser.y
	$(YACC) -d parser.y

clean :
	$(RM) $(TARGETS) *.o lex.yy.c y.tab.c y.tab.h
