# Vino Crescini
# 2002 May 30

CFLAGS = -O2 -Wall
TARGETS = main

all : $(TARGETS)

main : parser.tab.o scanner.o
	$(CC) $(CFLAGS) parser.tab.o scanner.o -ll -o $(@)

scanner.o : scanner.c parser.tab.h
	$(CC) $(CFLAGS) -c scanner.c -o $(@)

parser.tab.o : parser.tab.c parser.tab.h
	$(CC) $(CFLAGS) -c parser.tab.c -o $(@)

scanner.c : scanner.l
	$(LEX) -o$(@) scanner.l

parser.tab.c : parser.y
	$(YACC) -b parser parser.y

parser.tab.h : parser.y
	$(YACC) -b parser -d parser.y

clean :
	$(RM) $(TARGETS) *.o scanner.c parser.tab.c parser.tab.h
