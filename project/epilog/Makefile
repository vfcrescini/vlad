# Vino Crescini
# 2002 May 30

DIRS = parser
CFLAGS = -O3 -Wall -ansi

all :
	@for i in $(DIRS); do                     \
	  $(MAKE) CFLAGS="$(CFLAGS)" -C $$i all;  \
	done

clean :
	@for i in $(DIRS); do                     \
	  $(MAKE) -C $$i clean;                   \
	done
