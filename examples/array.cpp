#include <iostream>
#include "json.hpp"
#include "examples.hpp"

void array_example1() {

	JSON msg;
	msg[2] = "Hello";
	msg[3] = "World";
	msg[1] = "is";
	msg[0] = "This";

	msg.append("appended value");

	std::cout << "\narray example #1:\n" << msg << std::endl;
}

void array_example2() {

	JSON msg = JSON::Array("This", "is", "Hello", "World", "Again");
	std::cout << "\narray example #2:\n" << msg << std::endl;
}

void array_example3() {

	JSON msg = JSON({
		"Arr", JSON::Array("This", "was", "hello", "world")
	});
	std::cout << "\narray example #3: (nested)\n" << msg << std::endl;
}

void array_example4() {

	JSON msg;
	msg[2][0][1] = true;
	std::cout << "\narray example #4: (nested)\n" << msg << std::endl;
}

void array_example5() {

	JSON msg = JSON::Array( 2, "text", true );
	std::cout << "\narray example #5: (mixed values)\n" << msg << std::endl;
}

void array_example6() {

	JSON obj1 = JSON::Object();
	obj1["name"] = "John Doe";
	obj1["age"] = 42;

	JSON obj2 = JSON::Object();
	obj2["name"] = "Jane Doe";
	obj2["age"] = 40;

	JSON arr = JSON::Array();
	arr.append(obj1);
	arr.append(obj2);

	JSON msg({ "people", arr });

	std::cout << "\narray example #6:\n" << msg << std::endl;
}
