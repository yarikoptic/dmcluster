OBJS=$(patsubst %.cc,%.o, $(wildcard *.cc) )

CXXFLAGS=-O3 -g -Wall -Ibase/ -I /usr/include/nifti
LD_FLAGS+=-lniftiio

all: $(OBJS) base
	$(CXX) $(LD_FLAGS) -o cluster *.o base/*.o

base:
	$(MAKE) -C base

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) -o  $@ $<

.PHONY: base all clean
clean:
	$(MAKE) -C base clean
	rm -f *.o *.d *~
