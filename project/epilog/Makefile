# Vino Crescini
# 2002 May 30

DIRS = datastruct symtab parser

all :
	@for i in $(DIRS); do \
	$(MAKE) -C $$i all; \
	done

clean :
	@for i in $(DIRS); do \
	$(MAKE) -C $$i clean; \
	done
	$(RM) -r include lib
