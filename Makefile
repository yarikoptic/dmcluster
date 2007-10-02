
CXXFLAGS=-O2 -g -Wall -Ibase/

all:  cluster.o density.o base
	$(CXX) $(LD_FLAGS) -o cluster *.o base/*.o

base:
	$(MAKE) -C base

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) -o  $@ $<

.PHONY: base all clean
clean:
	$(MAKE) -C base clean
	rm -f *.o *.d *~
