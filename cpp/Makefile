.PHONY: all clean

all: bin bin/static_side_effect

bin:
	mkdir bin

bin/%: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f bin/*
