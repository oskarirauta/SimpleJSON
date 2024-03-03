#include <iostream>
#include "json.hpp"
#include "examples.hpp"

void tree_example1() {

	JSON msg({
		"value1", 10,
		"value2", nullptr,
		"value3", true,
		"value4", "text",
		"value5", {
			"sub-value1", 1,
			"sub-value2", 2
		},
		"value6", JSON::Array("This", "is", "array")
	});

	std::cout << "\ntree example #1:\n" << msg << std::endl;
}

void tree_example2() {

        JSON value = JSON::Load("\" \\\"Hello\\/world\\\" \"");
        JSON msg({ "value", value });
        std::cout << "\ntree example #2:\n" << msg << std::endl;
}

void tree_example3() {

	JSON obj = JSON::Object();
	obj["hello"] = "world";
	obj["array"] = JSON::Array("text", 10);

	JSON msg({
		"object", obj
	});

	std::cout << "\ntree example #3:\n" << msg << std::endl;
}
