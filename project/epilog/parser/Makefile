# Vino Crescini
# 2002 May 30

CFLAGS = -O2 -Wall
TARGETS = main

all : $(TARGETS)

main : parser.c scanner.c
	$(CC) $(CFLAGS) parser.c scanner.c -ll -o $(@)

scanner.c : scanner.l parser.h
	$(LEX) -o$(@) scanner.l

parser.c : parser.y
	$(YACC) -o$(@) parser.y

parser.h : parser.y
	$(YACC) -d -oparser.c parser.y

clean :
	$(RM) $(TARGETS) scanner.c scanner.o parser.c parser.o parser.h
