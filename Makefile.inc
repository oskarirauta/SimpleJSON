JSON_DIR?=json
INCLUDES += -I./$(JSON_DIR)/include
JSON_OBJS:= \
        objs/json.o

objs/json.o: $(JSON_DIR)/src/json.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;
