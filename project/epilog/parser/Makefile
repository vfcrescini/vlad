# Vino Crescini
# 2002 May 30

TARGETS = main

all : $(TARGETS)

main : scanner.c
	$(CC) $(CFLAGS) scanner.c -ll -o$(@)

scanner.c : scanner.l symbols.h
	$(LEX) -o$(@) scanner.l

clean :
	$(RM) $(TARGETS) scanner.c scanner.o
