# Vino Crescini <jcrescin@cit.uws.edu.au>

TOPSRCDIR = ..
CFLAGS = -ansi -Wall -Wstrict-prototypes -I$(TOPSRCDIR)/include -L$(TOPSRCDIR)/lib
INCLUDES = epilog.h

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

all : public

public :
	@mkdir -p $(TOPSRCDIR)/include
	@for i in $(INCLUDES); do \
	ln -sf ../common/$$i $(TOPSRCDIR)/include; \
	done

clean :
	@for i in $(INCLUDES); do \
	$(RM) $(TOPSRCDIR)/include/$$i; \
	done
