# Vino Crescini
# 2002 May 30

TOPSRCDIR = ..
CFLAGS = -O2 -Wall -I${TOPSRCDIR}/include -L${TOPSRCDIR}/lib
TARGETS = main

all : $(TARGETS)

main : parser.o scanner.o
	$(CC) $(CFLAGS) -o$(@) parser.o scanner.o -ll -lsymtab -ldatastruct

scanner.o : lex.yy.c y.tab.h
	$(CC) $(CFLAGS) -o$(@) -c lex.yy.c


parser.o : y.tab.c y.tab.h
	$(CC) $(CFLAGS) -o$(@) -c y.tab.c

lex.yy.c : scanner.l
	$(LEX) scanner.l

y.tab.c : parser.y
	$(YACC) parser.y

y.tab.h : parser.y
	$(YACC) -d parser.y

clean :

	$(RM) $(TARGETS) *.o lex.yy.c y.tab.c y.tab.h
