CFLAGS  = -O3 -Wall

PROGS	=            \
          loggerType \
          $(NULL)

all : $(PROGS)

loggerType : loggerType.o
	$(CXX) $(CFLAGS) -o $@ loggerType.cpp

loggerType.o : loggerType.cpp loggerType.h
	$(CXX) $(CFLAGS) -c loggerType.cpp

clean :
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS)
