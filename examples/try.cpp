#include "iostream"
#include "json.hpp"
#include "examples.hpp"

void try_example1() {

	// broken or un-parseable json, on failure, json will contain: null
	std::string json = "{\"hello: \"world\",";
	std::cout << "\ntrying to parse '" << json << "'";

	try {
		JSON msg = JSON::Load(json);
		std::cout << " with success\n" << msg << std::endl;
	} catch ( const std::exception& e ) {
		std::cout << "\nfailure with error message " << e.what() << "\n" << "null" << std::endl;
	}
}
