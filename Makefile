all: world
CXX?=g++
CXXFLAGS?=--std=c++17 -Wall
INCLUDES:=-I./include -I.

EXAMPLES:=json_example array_example prim_example init_example load_example iter_example tester
world: $(EXAMPLES)

objs/json_example.o: examples/json_example.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/array_example.o: examples/array_example.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/prim_example.o: examples/prim_example.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/init_example.o: examples/init_example.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/load_example.o: examples/load_example.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/iter_example.o: examples/iter_example.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/tester.o: test/tester.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

json_example: objs/json_example.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

array_example: objs/array_example.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

prim_example: objs/prim_example.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

init_example: objs/init_example.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

load_example: objs/load_example.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

iter_example: objs/iter_example.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

tester: objs/tester.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

clean:
	rm -f objs/*.o $(EXAMPLES)
