#include "iostream"
#include "json.hpp"
#include "examples.hpp"

void load_example1() {

	std::string json;
	json += "{\"hello\": \"world\", \"number\": 123, \"float\": 321.75, \"bool\": false,";
	json += " \"string\": \"test\", \"empty_string\": \"\", \"empty\": }";
	JSON msg = JSON::Load(json);
	std::cout << "\nload example #1:\n" << msg << std::endl;
}

void load_example2() {

	std::string json = "";
	json += "{\"note\": \"test for mixed tick types\",";
	json += "\"mixed_ticks\": 'value is singled ticked',";
	json += "'single_tick': 'this value\\\'s name is also singled ticked'}";
	JSON msg = JSON::Load(json);
        std::cout << "\nload example #2:\n" << msg << std::endl;
}

void load_example3() {

	JSON msg = JSON::Load("[1,2, true, false,\"STRING\", 1.5]");
	std::cout << "\nload example #3:\n" << msg << std::endl;
}
