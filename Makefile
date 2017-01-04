# Edmanuel Torres 2013

include makeinclude

all:	$(OBJS)
	echo "==== Compiling  ====";
	$(CXX) -o $(PROG)$(SUFX) $(OBJS) $(LFLAGS)


clean:
	echo "==== Cleaning ====";
	rm -rf *.o $(OBJS) $(PROG)$(SUFX)
