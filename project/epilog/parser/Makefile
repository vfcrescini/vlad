# Vino Crescini
# 2002 May 30

TARGETS = main

all : $(TARGETS)

main : parser.y scanner.c

scanner.c : scanner.l y.tab.h
	$(LEX) -o$(@) scanner.l

clean :
	$(RM) $(TARGETS) scanner.c scanner.o
