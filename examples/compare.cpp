#include <iostream>
#include "json.hpp"
#include "examples.hpp"

static JSON msg = JSON::Load("{'bool': false, 'str': 'text', 'int': 9, 'float': 235.99, 'null': null, 'arr':['value1', 'value2']}");

void comparison_print() {

	std::cout << "\njson to compare with:\n" << msg << std::endl;
}

void comparison_example1() {

	int i = msg["int"]; // We know it exists, so we don't need to cast it to int, there is operator for that
	int i2 = msg.contains("int2") ? (int)msg["int2"] : -1; // cast to int needed, otherwise there is a race issue

	std::cout << "\nis 'bool' true? " << ( msg["bool"] ? "yes" : "no" ) << std::endl;
	std::cout << "is 'bool' false? " << ( msg["bool"] == false ? "yes" : "no" ) << std::endl;
	std::cout << "'int' as int = " << i << std::endl;
	std::cout << "'int2' does not exist, we checked and assign it -1 if it doesn't exist, so, value is: " << i2 << std::endl;
	std::cout << "'float' is floating value (double), it's value is " << msg["float"] << std::endl;
	std::cout << "is it larger than " << (int)msg["float"] << "? ";
	if ( msg["float"] > (int)msg["float"] ) std::cout << "yes" << std::endl;
	else std::cout << "no" << std::endl;

	if ( !msg["null2"]) std::cout << "'null2' does not exist or is null" << std::endl;
	if ( msg["null"] == nullptr) std::cout << "'null' is null" << std::endl;

	// we cannot do msg["name"] to check if value exists because there is no reverse NOT operator
	// but we can do !! to get same result. Though, it's recommended to use '.contains("name")' instead..
	if ( !!msg["unknown"]) std::cout << "'unknown' exists" << std::endl;
	std::cout << "arr has " << msg["arr"].size() << " elements" << std::endl;
}

void cast_example1() {

	bool b = msg["bool"]; // or to_bool();
	std::string s = msg["str"]; // or to_string();
	int i = msg["int"]; // or to_int, or you can cast with (int)msg...
	double d = msg["float"]; // or to_double or by casting with (double);

	std::cout << "b = " << ( b ? "true" : "false" ) <<
			", s = " << s <<
			", i = " << i <<
			", d = " << std::to_string(d) <<
			", null = " <<
			( msg["null"].is_null() ? "null" : "not null" ) <<
			std::endl;
}

void iterate_example1() {

	std::cout << "\niterating over 'arr':";
	for ( auto &j : msg["arr"].ArrayRange())
		std::cout << "\n - " << j;
	std::cout << std::endl;
}

void iterate_example2() {

	std::cout << "\niterating over all json values:";
	for ( auto &j : msg.ObjectRange())
		std::cout << "\nObject[" << j.first << "] = " << j.second;
	std::cout << std::endl;
}
