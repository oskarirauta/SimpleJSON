#include "iostream"
#include "json.hpp"
#include "examples.hpp"

void value_example1() {

	JSON msg = JSON::Load(" 123 ");
	std::cout << "\nvalue example #1:\n" << msg << std::endl;
}

void value_example2() {

	std::string json = "\" \\\"Hello\\/world\\\" \"";
	JSON msg = JSON::Load(json);
	std::cout << "\nvalue example #2:\nsource string: '" << json << "'\njson: " << msg << std::endl;
}

void value_example3() {

        JSON msg = JSON::Load(" 123 ");
        std::cout << "\nvalue example #3: (json value to string)\n" << msg.to_string() << std::endl;
}

void value_example4() {

	JSON msg({ "string", "text", "float", "2.2", "int", "5", "bool", "true" });
	std::cout << "\nvalue example #4:\njson: " << msg << std::endl;
	std::cout << "to_bool: " << ( msg["bool"].to_bool() ? "true" : "false" ) << std::endl;
	std::cout << "to_float(double): " << msg["float"].to_float() << std::endl;
	std::cout << "to_int: " << msg["int"].to_int() << std::endl;
	std::cout << "Float value with to_int: " << msg["float"].to_int() << std::endl;
	std::cout << "to_string: " << msg["string"].to_string() << std::endl;
}

void value_example5() {

	JSON msg = JSON::Object();
	std::cout << "\nvalue example #5: (empty object)\n" << msg << std::endl;
}

void value_example6() {

	JSON msg = JSON::Object();
	msg["Key1"] = 1.0;
	msg["Key2"] = "Value";
	JSON obj = JSON::Object();
	obj["Key3"] = 1;
	msg["Key6"] = obj;

	std::cout << "\nvalue example #6:\n" << msg << std::endl;
}

void value_example7() {

	JSON msg({ "value1", 1, "value2", 2 });
	msg["value3"] = JSON::Make(JSON::Class::String); // empty string type
	std::cout << "\nvalue example #7:\n" << msg << std::endl;
}
