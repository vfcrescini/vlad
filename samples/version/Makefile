# Makefile
# Vino Crescini
# 1 Jan 2002

CFLAGS=-Wall -O3

all : stripped

stripped : version
	strip version

version : version.c
	$(CC) $(CFLAGS) version.c -o version

clean :
	rm -f version *.o
