.PHONY: all clean

all: bin tmp bin/static_side_effect bin/bypass_destructor

bin:
	mkdir bin

tmp:
	mkdir tmp

bin/%: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f bin/*
