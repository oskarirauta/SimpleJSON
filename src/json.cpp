#include "json.hpp"

const JSON::error_category json_error_category;

std::string JSON::json_escape(const std::string &str) {

	std::string output;

	for ( unsigned i = 0; i < str.length(); ++i ) {

		switch ( str[i] ) {
			case '\"':
				output += "\\\"";
				break;
			case '\\':
				output += "\\\\";
				break;
			case '\b':
				output += "\\b";
				break;
			case '\f':
				output += "\\f";
				break;
			case '\n':
				output += "\\n";
				break;
			case '\r':
				output += "\\r";
				break;
			case '\t':
				output += "\\t";
				break;
			default:
				output += str[i];
				break;
		}
	}

	return output;
}

std::string JSON::error_category::message(int ev) const {

	switch (static_cast<JSON::error > (ev)) {
		case JSON::error::float_conversion_failed_invalid_arg:
			return "Failed to convert the value to float: Invalid argument!";
		case JSON::error::float_conversion_failed_out_of_range:
			return "Failed to convert the value to float: Out of range!";
		case JSON::error::float_conversion_failed:
			return "Failed to convert the value to float!";
		case JSON::error::object_missing_colon:
			return "Parsing Object failed: Expected colon not found!";
		case JSON::error::object_missing_comma:
			return "Parsing Object failed: Expected comma not found!";
		case JSON::error::array_missing_comma_or_bracket:
			return "Parsing Array failed: Expected ',' or ']' not found!";
		case JSON::error::string_missing_hex_char:
			return "Parsing String failed: Expected hex character in unicode escape not found!";
		case JSON::error::string_conversion_failed:
			return "Failed to convert the value to string!";
		case JSON::error::string_unescaped_conversion_failed:
			return "Failed to convert the value to a unescaped string!";
		case JSON::error::number_missing_exponent:
			return "Parsing Number failed: Expected number for exponent not found!";
		case JSON::error::number_unexpected_char:
			return "Parsing Number failed: Unexpected character!";
		case JSON::error::number_conversion_failed:
			return "Failed to convert the value to int!";
		case JSON::error::bool_wrong_text:
			return "Parsing Bool failed: Expected 'true' or 'false' not found!";
		case JSON::error::bool_conversion_failed:
			return "Failed to convert the value to boolean!";
		case JSON::error::null_wrong_text:
			return "Parsing Null failed: Expected 'null' not found!";
		case JSON::error::unknown_starting_char:
			return "Parsing failed: Unknown starting character!";
		default:
			return "Unrecognized error occured...";
	}
}

JSON parse_next(const std::string &, size_t &, std::error_code &) noexcept;

void consume_ws(const std::string &str, size_t &offset) {
	while (isspace(str[offset])) ++offset;
}

JSON parse_object(const std::string &str, size_t &offset, std::error_code &ec) noexcept {

	JSON Object = JSON::Make(JSON::Class::Object);

	++offset;
	consume_ws(str, offset);
	if ( str[offset] == '}' ) {
		++offset;
		return Object;
	}

	while (true) {

		JSON Key = parse_next(str, offset, ec);
		consume_ws(str, offset);
		if ( str[offset] != ':' ) {
			ec = JSON::make_error_code(JSON::error::object_missing_colon);
			break;
		}

		consume_ws(str, ++offset);
		JSON Value = parse_next(str, offset, ec);
		Object[Key.to_string()] = Value;

		consume_ws(str, offset);
		if ( str[offset] == ',' ) {
			++offset;
			continue;
		} else if ( str[offset] == '}' ) {
			++offset;
			break;
		} else {
			JSON::make_error_code(JSON::error::object_missing_comma);
			break;
		}
	}

	return Object;
}

JSON parse_array(const std::string &str, size_t &offset, std::error_code &ec) noexcept {

	JSON Array = JSON::Make(JSON::Class::Array);
	unsigned index = 0;

	++offset;
	consume_ws(str, offset);
	if ( str[offset] == ']' ) {
		++offset;
		return Array;
	}

	while (true) {

		Array[index++] = parse_next(str, offset, ec);
		consume_ws(str, offset);

		if ( str[offset] == ',' ) {
			++offset;
			continue;
		} else if ( str[offset] == ']' ) {
			++offset;
			break;
		} else {
			JSON::make_error_code(JSON::error::array_missing_comma_or_bracket);
			return JSON::Make(JSON::Class::Array);
		}
	}
	return Array;
}

JSON parse_string(const std::string &str, char tick, size_t &offset, std::error_code &ec) noexcept {

	std::string val;
	for ( char c = str[++offset]; c != tick; c = str[++offset] ) {

		if ( c == '\\') {
			switch (str[++offset]) {
				case '\'':
					val += '\'';
					break;
				case '\"':
					val += '\"';
					break;
				case '\\':
					val += '\\';
					break;
				case '/':
					val += '/';
					break;
				case 'b':
					val += '\b';
					break;
				case 'f':
					val += '\f';
					break;
				case 'n':
					val += '\n';
					break;
				case 'r':
					val += '\r';
					break;
				case 't':
					val += '\t';
					break;
				case 'u': {
					val += "\\u";
					for (unsigned i = 1; i <= 4; ++i) {
						c = str[offset + i];
						if (( c >= '0' && c <= '9') || ( c >= 'a' && c <= 'f') || ( c >= 'A' && c <= 'F'))
							val += c;
						else {
							JSON::make_error_code(JSON::error::string_missing_hex_char);
							return JSON::Make(JSON::Class::String);
						}
					}

					offset += 4;
				} break;
				default:
					val += '\\';
					break;
			}
		} else val += c;
	}
	++offset;
	return JSON(val);
}

JSON parse_number(const std::string &str, size_t &offset, std::error_code &ec) noexcept {

	JSON Number;
	char c;
	bool isDouble = false;
	long long exp = 0;
	std::string val, exp_str;

	while (true) {

		c = str[offset++];

		if (( c == '-') || ( c >= '0' && c <= '9' ))
			val += c;
		else if ( c == '.' ) {
			val += c;
			isDouble = true;
		} else break;
	}

	if ( c == 'E' || c == 'e' ) {
		c = str[offset];
		if ( c == '-' ) {
			++offset;
			exp_str += '-';
		}

		if ( c == '+' ) {
			++offset;
		}

		while (true) {

			c = str[offset++];
			if ( c >= '0' && c <= '9' )
				exp_str += c;
			else if ( !isspace(c) && c != ',' && c != ']' && c != '}' ) {
				JSON::make_error_code(JSON::error::number_missing_exponent);
				return JSON::Make(JSON::Class::Null);
			} else break;
		}

		exp = std::stol(exp_str);

	} else if ( !isspace(c) && c != ',' && c != ']' && c != '}' ) {
		JSON::make_error_code(JSON::error::number_unexpected_char);
		return JSON::Make(JSON::Class::Null);
	}

	--offset;

	if ( isDouble )
		Number = std::stod(val) *std::pow(10, exp);
	else {
		if ( !exp_str.empty())
			Number = std::stol(val) * std::pow(10, exp);
		else
			Number = std::stol(val);
		}
	return Number;
}

JSON parse_bool(const std::string &str, size_t &offset, std::error_code &ec) noexcept {

	JSON Bool;

	if ( str.substr(offset, 4) == "true" )
		Bool = true;
	else if ( str.substr(offset, 5) == "false" )
		Bool = false;
	else {
		JSON::make_error_code(JSON::error::bool_wrong_text);
		return JSON::Make(JSON::Class::Null);
	}
	offset += (Bool.to_bool() ? 4 : 5);
	return Bool;
}

JSON parse_null(const std::string &str, size_t &offset, std::error_code &ec) noexcept {

	if ( str.substr(offset, 4) != "null" ) {
		JSON::make_error_code(JSON::error::null_wrong_text);
		return JSON::Make(JSON::Class::Null);
	}
	offset += str.substr(offset, 7) == "nullptr" ? 7 : 4;
	return JSON();
}

JSON parse_next(const std::string &str, size_t &offset, std::error_code &ec) noexcept {

	char value;
	consume_ws(str, offset);
	value = str[offset];

	switch (value) {
		case '[':
			return parse_array(str, offset, ec);
		case '{':
			return parse_object(str, offset, ec);
		case '\"':
			return parse_string(str, '\"', offset, ec);
		case '\'':
			return parse_string(str, '\'', offset, ec);
		case 't':
		case 'f':
			return parse_bool(str, offset, ec);
		case 'n':
			return parse_null(str, offset, ec);
		default:
			if (( value <= '9' && value >= '0') || value == '-' )
				return parse_number(str, offset, ec);
	}

	JSON::make_error_code(JSON::error::unknown_starting_char);
	return JSON();
}

JSON JSON::Load(const std::string &str, std::error_code &ec) noexcept {

	size_t offset = 0;
	return parse_next(str, offset, ec);
}

JSON JSON::Load(const std::string &str) {

	size_t offset = 0;
	std::error_code ec;
	if ( JSON obj = parse_next(str, offset, ec); !ec )
		return obj;
	else throw std::runtime_error(ec.message());
}

