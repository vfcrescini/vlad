CFLAGS  = -O3 -Wall
STRIP	= strip --strip-all
PROGS	=                  \
          libloggertype.so \
          $(NULL)

all : $(PROGS)

strip-all : $(PROGS) strip

libloggertype.so : loggerType.o
	$(CXX) $(CFLAGS) -shared -o $@ loggerType.cpp

loggerType.o : loggerType.cpp loggerType.h
	$(CXX) $(CFLAGS) -c -fPIC -o $@ loggerType.cpp
	
strip : $(PROGS)
	$(STRIP) $(PROGS)

clean :
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS)
