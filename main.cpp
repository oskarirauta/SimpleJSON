#include <iostream>
#include "json.hpp"
#include "examples.hpp"

int main(int argc, char **argv) {

	value_example1();
	value_example2();
	value_example3();
	value_example4();
	value_example5();
	value_example6();
	value_example7();

	array_example1();
	array_example2();
	array_example3();
	array_example4();
	array_example5();
	array_example6();

	tree_example1();
	tree_example2();
	tree_example3();

	load_example1();
	load_example2();
	load_example3();

	comparison_print();
	comparison_example1();
	cast_example1();
	iterate_example1();
	iterate_example2();

	// Throw and catch is also supported for most features
	try_example1();

	return 0;
}
