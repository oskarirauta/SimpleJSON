all: world
CXX?=g++
CXXFLAGS?=--std=c++17 -Wall -fPIC -g
LDFLAGS?=-L/lib -L/usr/lib

INCLUDES+= -I./examples/include

OBJS:= \
	objs/value.o \
	objs/array.o \
	objs/tree.o \
	objs/load.o \
	objs/compare.o \
	objs/try.o \
	objs/main.o

JSON_DIR:=.
include ./Makefile.inc

world: example

$(shell mkdir -p objs)

objs/value.o: examples/value.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/array.o: examples/array.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/tree.o: examples/tree.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/load.o: examples/load.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/compare.o: examples/compare.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/try.o: examples/try.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

example: $(JSON_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -L. $(LIBS) $^ -o $@;

.PHONY: clean
clean:
	@rm -rf objs
	@rm -f example
