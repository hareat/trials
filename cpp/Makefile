.PHONY: all boost cpp clean

# per default only build the standard C++ binaries
cpp: bin tmp bin/bypass_destructor bin/global_mess bin/global_mess2 bin/malloc_ptr bin/static_side_effect

boost: bin tmp bin/boost-mutex

all: cpp boost

bin:
	mkdir bin

tmp:
	mkdir tmp

bin/boost-mutex: boost-mutex.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -lboost_thread -o $@

bin/%: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f bin/*
