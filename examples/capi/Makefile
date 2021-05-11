#  _____________________________________________________________________________
#  Copyright (c) 2018-2021 Steven Varga, Toronto,ON Canada
#  Author: Varga, Steven <steven@vargaconsulting.ca>
#  _____________________________________________________________________________

CXXFLAGS =  -std=c++17 -Wno-attributes
LIBS =  -lhdf5  -lz -ldl -lm
INCLUDES = -I/usr/local/include -I/usr/include -I../../include

programs=capi-test
test: $(programs)
benchmark: capi-bench

%.o : %.cpp 
	$(CXX) $(INCLUDES) -o $@  $(CPPFLAGS) $(CXXFLAGS) -c $^

%-test: %-test.o 
	$(CXX) $^ $(LIBS) -o $@
	taskset 0x1 ./$@

%-bench: %-bench.o 
	$(CXX) $^ $(LIBS) -o $@

clean:
	@$(RM) *.o *.prof *.h5 *-test *-bench

dist-clean: clean

.PHONY: test

