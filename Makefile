OBJS=$(patsubst %.cc,%.o, $(wildcard *.cc) )

CPPFLAGS= -O3 -g -Wall -Ibase/ -I /usr/include/nifti
LD_FLAGS+=-lniftiio

# unobtrusively account for #include dependencies
override CPPFLAGS += -MMD
dfiles = $(addsuffix .d, $(basename $(notdir $(OBJS))))

all: $(OBJS) base
	$(CXX) $(LD_FLAGS) -o dmcluster *.o base/*.o

base:
	$(MAKE) -C base

ifneq ($(dfiles),)
-include $(dfiles)
endif


.PHONY: base all clean
clean:
	$(MAKE) -C base clean
	rm -f *.o *.d *~ $(dfiles) cluster
