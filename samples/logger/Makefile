CFLAGS  = -O3 -Wall
STRIP	= strip --strip-all
PROGS	=                  \
          libloggertype.so \
          $(NULL)

all : $(PROGS)

libloggertype.so : loggerType.o
	$(CXX) $(CFLAGS) -shared -o $@ loggerType.cpp
	$(STRIP) $(PROGS)

loggerType.o : loggerType.cpp loggerType.h
	$(CXX) $(CFLAGS) -c -fPIC -o $@ loggerType.cpp
	
clean :
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS)
