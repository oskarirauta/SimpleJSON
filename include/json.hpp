#pragma once

//#include <cstdint>
#include <cmath>
//#include <cctype>
#include <string>
#include <deque>
#include <map>
//#include <type_traits>
//#include <initializer_list>
//#include <ostream>
#include <charconv>
#include <system_error>
#include <stdexcept>

/**
 *@brief Class to represent and use JSON objects. Class may throw exceptions of type
 *std::error_code on error.
 */
class JSON final {

	private:

	/**
	 *@param str String to escape
	 *@returns A escaped version of the given string.
	 */
	static std::string json_escape(const std::string &str);

	union BackingData {

		BackingData(double d): Float(d) {}
		BackingData(long long l): Int(l) {}
		BackingData(bool b): Bool(b) {}
		BackingData(std::string s): String(new std::string(s)) {}
		BackingData(): Int(0) {}

		std::deque<JSON> *List;
		std::map<std::string, JSON> *Map;
		std::string * String;
		double Float;
		long long Int;
		bool Bool;

	} Internal;

	public:

	/**
	 *@brief Enum class to identify parsing and conversion errors.
	 */
	enum class error {
		float_conversion_failed_invalid_arg = 42,
		float_conversion_failed_out_of_range,
		float_conversion_failed,
		object_missing_colon,
		object_missing_comma,
		array_missing_comma_or_bracket,
		string_missing_hex_char,
		string_conversion_failed,
		string_unescaped_conversion_failed,
		number_missing_exponent,
		number_unexpected_char,
		number_conversion_failed,
		bool_wrong_text,
		bool_conversion_failed,
		null_wrong_text,
		unknown_starting_char
	};

	/**
	 *@brief Error category object used to convert the error code to a understandable message
	 *via std::error_code.
	 */


	struct error_category: std::error_category {

		const char *name() const noexcept override {
			return "JSON";
		}

		std::string message(int ev) const override;
	};


	/*
	 *Instance of json::error_category, can be reused, no need to create multiple instances
	 */
	inline static const JSON::error_category json_error_category;

	/**
	 *This overload makes json::error assignable to std::error_code
	 *@param e json::error to construct a std::error_code for.
	 *@returns std::error_code constructed from json::error
	 */

	inline static std::error_code make_error_code(JSON::error e) noexcept {
		return { static_cast<int> (e), json_error_category };
	};

	enum class Class {
		Null,
		Object,
		Array,
		String,
		Floating,
		Integral,
		Boolean
	};

	inline static JSON Array() {
		return JSON::Make(JSON::Class::Array);
	}

	template <typename...T >
	inline static JSON Array(T...args) {
		JSON arr = JSON::Make(JSON::Class::Array);
		arr.append(args...);
		return arr;
	}

	inline static JSON Object() {
		return JSON::Make(JSON::Class::Object);
	}

	/**
	 *@brief Provides iterators to iterate over objects/arrays.
	 */
	template <typename Container>
	class JSONWrapper {

		private:
		Container* object;

		public:

		JSONWrapper(Container *val): object(val) {}
		JSONWrapper(std::nullptr_t): object(nullptr) {}

		typename Container::iterator begin() {
			return object ? object -> begin() : typename Container::iterator();
		}

		typename Container::iterator end() {
			return object ? object -> end() : typename Container::iterator();
		}

		typename Container::const_iterator begin() const {
			return object ? object -> begin() : typename Container::iterator();
		}

		typename Container::const_iterator end() const {
			return object ? object -> end() : typename Container::iterator();
		}
	};

	/**
	 *@brief Provides const iterators to iterate over objects/arrays.
	 */
	template <typename Container>
	class JSONConstWrapper {

		private:
		const Container * object;

		public:
		JSONConstWrapper(const Container *val): object(val) {}
		JSONConstWrapper(std::nullptr_t): object(nullptr) {}

		typename Container::const_iterator begin() const {
			return object ? object -> begin() : typename Container::const_iterator();
		}

		typename Container::const_iterator end() const {
			return object ? object -> end() : typename Container::const_iterator();
		}
	};

	JSON(): Internal(), Type(Class::Null) {}

	explicit JSON(Class type): JSON() {
		SetType(type);
	}

	JSON(std::initializer_list<JSON> list): JSON() {
		SetType(Class::Object);
		for ( auto i = list.begin(), e = list.end(); i != e; ++i, ++i )
			operator[](i -> to_string()) = *std::next(i);
	}

	JSON(JSON &&other): Internal(other.Internal), Type(other.Type) {
		other.Type = Class::Null;
		other.Internal.Map = nullptr;
	}

	JSON &operator=(JSON && other) {
		ClearInternal();
		Internal = other.Internal;
		Type = other.Type;
		other.Internal.Map = nullptr;
		other.Type = Class::Null;
		return *this;
	}

	JSON(const JSON &other) {
		switch (other.Type) {
			case Class::Object:
				Internal.Map =
					new std::map<std::string, JSON>(other.Internal.Map -> begin(),
						other.Internal.Map -> end());
				break;
			case Class::Array:
				Internal.List =
					new std::deque<JSON>(other.Internal.List -> begin(),
						other.Internal.List -> end());
				break;
			case Class::String:
				Internal.String =
					new std::string(*other.Internal.String);
				break;
			default:
				Internal = other.Internal;
		}

		Type = other.Type;
	}

	JSON &operator=(const JSON &other) {
		if ( &other == this ) return *this;
		ClearInternal();
		switch (other.Type) {
			case Class::Object:
				Internal.Map =
					new std::map<std::string, JSON> (other.Internal.Map -> begin(),
						other.Internal.Map -> end());
				break;
			case Class::Array:
				Internal.List =
					new std::deque<JSON> (other.Internal.List -> begin(),
						other.Internal.List -> end());
				break;
			case Class::String:
				Internal.String =
					new std::string(*other.Internal.String);
				break;
			default:
				Internal = other.Internal;
		}

		Type = other.Type;
		return *this;
	}

	~JSON() {
		switch (Type) {
			case Class::Array:
				delete Internal.List;
				break;
			case Class::Object:
				delete Internal.Map;
				break;
			case Class::String:
				delete Internal.String;
				break;
			default: ;
		}
	}

	template <typename T>
	JSON(T b, typename std::enable_if<std::is_same<T, bool>::value>::type* = 0): Internal(b), Type(Class::Boolean) {}

	template <typename T>
	JSON(T i, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = 0): Internal((long long) i), Type(Class::Integral) {}

	template <typename T>
	JSON(T f, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0): Internal((double) f), Type(Class::Floating) {}

	template <typename T>
	JSON(T s, typename std::enable_if<std::is_convertible<T, std::string >::value>::type* = 0): Internal(std::string(s)), Type(Class::String) {}

	JSON(std::nullptr_t): Internal(), Type(Class::Null) {}

	/**
	 *Creates a new JSON object.
	 *@param type Class type to create.
	 *@returns JSON object of given class type.
	 */
	static JSON Make(Class type) {
		return JSON(type);
	}

	/**
	 *Create a JSON object from string, throws std::error_code on error.
	 *@param str JSON string to parse and load.
	 *@returns New JSON object representing the json defined by the parsed string.
	 */
	static JSON Load(const std::string &str);

	/**
	 *Create a JSON object from string.
	 *@param str JSON string to parse and load.
	 *@param ec[OUT] Output parameter giving feedback if parsing was successful.
	 *@returns New JSON object representing the json defined by the parsed string.
	 */
	static JSON Load(const std::string &str, std::error_code &ec) noexcept;

	/**
	 *Allows appending items to array. Appending to a non-array will turn the object into an array with the
	 *first element being the value that's being appended.
	 *@param arg Item to append.
	 */
	template <typename T>
	void append(T arg) {
		SetType(Class::Array);
		Internal.List -> emplace_back(arg);
	}

	/**
	 *Allows appending items to array. Appending to a non-array will turn the object into an array with the
	 *first element being the value that's being appended.
	 *@param arg Item to append.
	 *@param args Further items to append.
	 */
	template <typename T, typename...U >
	void append(T arg, U...args) {
		append(arg);
		append(args...);
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, bool>::value, JSON &>::type operator=(T b) {
		SetType(Class::Boolean);
		Internal.Bool = b;
		return *this;
	}

	template <typename T>
	typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, JSON &>::type operator=(T i) {
		SetType(Class::Integral);
		Internal.Int = i;
		return *this;
	}

	template <typename T>
	typename std::enable_if<std::is_floating_point<T>::value, JSON &>::type operator=(T f) {
		SetType(Class::Floating);
		Internal.Float = f;
		return *this;
	}

	template <typename T>
	typename std::enable_if<std::is_convertible<T, std::string >::value, JSON &>::type operator=(T s) {
		SetType(Class::String); *Internal.String = std::string(s);
		return *this;
	}

	/**
	 *Allows accessing and creating object entries by key.
	 *@param key Key to access, will be created if not existent.
	 *@returns The object stored at key.
	 */
	JSON &operator[](const std::string &key) {
		SetType(Class::Object);
		return Internal.Map -> operator[](key);
	}

	/**
	 *Allows accessing and creating array entries by index.
	 *@param index Index to access, will be created if not existent.
	 *@returns The object stored at index.
	 */
	JSON &operator[](unsigned index) {
		SetType(Class::Array);
		if ( index >= Internal.List -> size())
			Internal.List -> resize(index + 1);
		return Internal.List -> operator[](index);
	}

	/**
	 *Allows getting an object entry by key.
	 *@param key Key to access.
	 *@returns object entry by key.
	 */
	JSON &at(const std::string &key) {
		return operator[](key);
	}

	/**
	 *Allows getting an object entry by key.
	 *@param key Key to access.
	 *@returns object entry by key.
	 */
	const JSON &at(const std::string &key) const {
		return Internal.Map -> at(key);
	}

	/**
	 *Allows getting an array entry by index.
	 *@param index Index to access.
	 *@returns array entry by index.
	 */
	JSON &at(unsigned index) {
		return operator[](index);
	}

	/**
	 *Allows getting an array entry by index.
	 *@param index Index to access.
	 *@returns array entry by index.
	 */
	const JSON &at(unsigned index) const {
		return Internal.List -> at(index);
	}

	/**
	 *@returns The number of items stored within an Array. -1 if 
	 *class type is not Array.
	 */
	std::size_t length() const {
		return Type == Class::Array ? Internal.List -> size() : -1;
	}

	/**
	 *@param key Key to check.
	 *@returns true if the object holds a item with the given key, false otherwise.
	 */
	bool hasKey(const std::string &key) const {
		return Type == Class::Object ? Internal.Map -> find(key) != Internal.Map -> end() : false;
	}

	bool contains(const std::string &key) const {
		return Type == Class::Object ? Internal.Map -> find(key) != Internal.Map -> end() : false;
	}

	/**
	 *@returns The number of items stored within an array or object. -1 if 
	 *class type is neither array nor object.
	 */
	std::size_t size() const {
		if ( Type == Class::Object)
			return Internal.Map -> size();
		else if ( Type == Class::Array)
			return Internal.List -> size();
		else
			return -1;
	}

	/**
	 *@returns Class type of the object.
	 */
	Class JSONType() const {
		return Type;
	}

	/**
	 *@returns true if the object is Null, false otherwise.
	 */
	bool is_null() const {
		return Type == Class::Null;
	}

	bool is_nullptr() const {
		return Type == Class::Null;
	}

	/**
	 *@returns true if the object is Array, false otherwise.
	 */
	bool is_array() const {
		return Type == Class::Array;
	}

	/**
	 *@returns true if the object is Boolean, false otherwise.
	 */
	bool is_boolean() const {
		return Type == Class::Boolean;
	}

	bool is_bool() const {
		return Type == Class::Boolean;
	}

	/**
	 *@returns true if the object is Floating, false otherwise.
	 */
	bool is_floating() const {
		return Type == Class::Floating;
	}

	bool is_float() const {
		return Type == Class::Floating;
	}

	bool is_double() const {
		return Type == Class::Floating;
	}

	/**
	 *@returns true if the object is Integral, false otherwise.
	 */
	bool is_integral() const {
		return Type == Class::Integral;
	}

	bool is_int() const {
		return Type == Class::Integral;
	}

	/**
	 *@returns true if the object is String, false otherwise.
	 */
	bool is_string() const {
		return Type == Class::String;
	}

	/**
	 *@returns true if the object is a JSONObject, false otherwise.
	 */
	bool is_object() const {
		return Type == Class::Object;
	}

	/**
	 *@param ec[OUT] Output parameter giving feedback if the conversion was successful.
	 *@returns If class type is String, the stored value. If class type is
	 *Null, JSONObject, Array, Boolean, Floating or Integral a conversion will be tried. 
	 *Comma of a converted Floating value depends on the users locale setting.
	 *Returns empty string otherwise or on conversion error.
	 */
	std::string to_string(std::error_code &ec) const noexcept {

		switch (Type) {
			case Class::String:
				return json_escape(*Internal.String);
			case Class::Object:
			case Class::Array:
				return dumpMinified();
			case Class::Boolean:
				return std::string(Internal.Bool ? "true" : "false");
			case Class::Floating:
				return std::to_string(Internal.Float);
			case Class::Integral:
				return std::to_string(Internal.Int);
			case Class::Null:
				return std::string("null");
		}

		ec = JSON::make_error_code(JSON::error::string_conversion_failed);
		return std::string("");
	}

	/**
	 *@returns If class type is String, the stored value. If class type is
	 *Null, JSONObject, Array, Boolean, Floating or Integral a conversion will be tried. 
	 *Comma of a converted Floating value depends on the users locale setting. Throws std::error_code
	 *on conversion error.
	 */
	std::string to_string() const {
		std::error_code ec;
		if ( std::string ret = to_string(ec); !ec )
			return ret;
		else throw std::runtime_error(ec.message());;
	}

	operator std::string() const {
		std::error_code ec;
		if ( std::string ret = to_string(ec); !ec )
			return ret;
		else throw std::runtime_error(ec.message());;
	}

	/**
	 *Useful if json objects are stored within the json as string.
	 *@param ec[OUT] Output parameter giving feedback if the conversion was successful.
	 *@returns If class type is String, the stored value without escaping. If class type is
	 *Null, JSONObject, Array, Boolean, Floating or Integral a conversion will be tried. Comma of a 
	 *converted Floating value depends on the users locale setting. Returns empty string otherwise or on conversion error.
	 */
	std::string to_unescaped_string(std::error_code &ec) const noexcept {

		switch (Type) {
			case Class::String:
				return std::string(*Internal.String);
			case Class::Object:
			case Class::Array:
				return dumpMinified();
			case Class::Boolean:
				return std::string(Internal.Bool ? "true" : "false");
			case Class::Floating:
				return std::to_string(Internal.Float);
			case Class::Integral:
				return std::to_string(Internal.Int);
			case Class::Null:
				return std::string("null");
		}

		ec = JSON::make_error_code(JSON::error::string_unescaped_conversion_failed);
		return std::string("");
	}

	/**
	 *Useful if json objects are stored within the json as string.
	 *@returns If class type is String, the stored value without escaping. If class type is
	 *Null, JSONObject, Array, Boolean, Floating or Integral a conversion will be tried. Comma of a 
	 *converted Floating value depends on the users locale setting. Throws std::error_code on conversion error.
	 */
	std::string to_unescaped_string() const {
		std::error_code ec;
		if ( std::string ret = to_unescaped_string(ec); !ec )
			return ret;
		else throw std::runtime_error(ec.message());;
	}

	/**
	 *@param ec[OUT] Output parameter giving feedback if the conversion was successful.
	 *@returns If class type is Integral, Floating or Boolean, the stored value. If the class type is
	 *String, an conversion will be tried. 0.0 otherwise or on conversion error.
	 */
	double to_float(std::error_code &ec) const noexcept {

		switch (Type) {
			case Class::Floating:
				return Internal.Float;
			case Class::Boolean:
				return Internal.Bool;
			case Class::Integral:
				return static_cast<double> (Internal.Int);
			case Class::String: {
				double parsed;
				try {
					parsed = std::stod(*Internal.String);
				} catch (const std::invalid_argument &e) {
					(void) e;
					ec = JSON::make_error_code(JSON::error::float_conversion_failed_invalid_arg);
				} catch (const std::out_of_range &e) {
					(void) e;
					ec = JSON::make_error_code(JSON::error::float_conversion_failed_out_of_range);
				}

				if ( !ec ) return parsed;
			}
			default:;
		}

		ec = JSON::make_error_code(JSON::error::float_conversion_failed);
		return 0.0;
	}

	/**
	 *@returns If class type is Integral, Floating or Boolean, the stored value. If the class type is
	 *String, an conversion will be tried. Throws std::error_code on conversion error.
	 */
	double to_float() const {
		std::error_code ec;
		if ( double ret = to_float(ec); !ec )
			return ret;
		else throw std::runtime_error(ec.message());;
	}

	operator double() const {

		if ( is_null())
			return 0;
		std::error_code ec;
		if ( double ret = to_float(ec); !ec )
			return ret;
		else throw std::runtime_error(ec.message());;
	}

	/**
	 *@param ec[OUT] Output parameter giving feedback if the conversion was successful.
	 *@returns If class type is Integral, Floating or Boolean, the stored value. If the class type is
	 *String, an conversion will be tried. 0 otherwise or on conversion error.
	 */
	long long to_int(std::error_code &ec) const noexcept {

		switch (Type) {
			case Class::Integral:
				return Internal.Int;
			case Class::Boolean:
				return Internal.Bool;
			case Class::Floating:
				return static_cast< long long > (Internal.Float);
			case Class::String: {
				long long parsed;
				std::from_chars_result result = std::from_chars(Internal.String -> data(), Internal.String -> data() + Internal.String -> size(), parsed);
				if ( !(bool) result.ec)
					return parsed;
			}
			default:;
		}

		ec = JSON::make_error_code(JSON::error::number_conversion_failed);
		return 0;
	}

	/**
	 *@returns If class type is Integral, Floating or Boolean, the stored value. If the class type is
	 *String, an conversion will be tried. Throws std::error_code on conversion error.
	 */
	long long to_int() const {
		std::error_code ec;
		if ( long long ret = to_int(ec); !ec )
			return ret;
		else throw std::runtime_error(ec.message());;
	}

	/**
	 *@param ok[OUT] Output parameter giving feedback if the conversion was successful.
	 *@returns If class type is Integral, Floating or Boolean, the stored value. If the class type is
	 *String, an conversion will be tried. false otherwise or on conversion error.
	 */
	bool to_bool(std::error_code &ec) const noexcept {

		switch (Type) {
			case Class::Boolean:
				return Internal.Bool;
			case Class::Integral:
				return Internal.Int;
			case Class::Floating:
				return Internal.Float;
			case Class::String: {
				if ( Internal.String -> find("true") != std::string::npos)
					return true;
				if ( Internal.String -> find("false") != std::string::npos)
					return false;
				int parsed;
				std::from_chars_result result = std::from_chars(Internal.String -> data(), Internal.String -> data() + Internal.String -> size(), parsed);
				if ( !(bool) result.ec)
					return parsed;
			}
			default:;
		}

		ec = JSON::make_error_code(JSON::error::bool_conversion_failed);
		return false;
	}

	/**
	 *@returns If class type is Integral, Floating or Boolean, the stored value. If the class type is
	 *String, an conversion will be tried. Throws std::error_code on conversion error.
	 */
	bool to_bool() const {
		std::error_code ec;
		if ( bool ret = to_bool(ec); !ec )
			return ret;
		else throw std::runtime_error(ec.message());
	}

	bool operator==(const std::string &s) const {
		return operator std::string() == s;
	}

	bool operator==(const std::nullptr_t &n) const {
		return is_null();
	}

	bool operator!() const {
		if ( is_null()) return true;
		if ( is_bool()) return to_bool() == false;
		return false;
	}

	/**
	 *Returns ObjectRange which allows iterating over the object items.
	 *@returns ObjectRange which allows iterating over the object items.
	 */
	JSONWrapper<std::map<std::string, JSON>> ObjectRange() {
		return Type == Class::Object ? JSONWrapper<std::map<std::string, JSON>> (Internal.Map) :
				JSONWrapper<std::map<std::string, JSON>> (nullptr);
	}

	/**
	 *Returns Array range which allows iterating over the array items.
	 *@returns Array range which allows iterating over the array items.
	 */
	JSONWrapper<std::deque < JSON>> ArrayRange() {
		return Type == Class::Array ? JSONWrapper<std::deque < JSON>> (Internal.List) :
			JSONWrapper<std::deque < JSON>> (nullptr);
	}

	/**
	 *Returns ObjectRange which allows iterating over the object items.
	 *@returns ObjectRange which allows iterating over the object items.
	 */
	JSONConstWrapper<std::map<std::string, JSON>> ObjectRange() const {
		return Type == Class::Object ? JSONConstWrapper<std::map<std::string, JSON>> (Internal.Map) :
			JSONConstWrapper<std::map<std::string, JSON>> (nullptr);
	}

	/**
	 *Returns ArrayRange which allows iterating over the array items.
	 *@returns ArrayRange which allows iterating over the array items.
	 */
	JSONConstWrapper<std::deque < JSON>> ArrayRange() const {
		return Type == Class::Array ? JSONConstWrapper<std::deque < JSON>> (Internal.List) :
			JSONConstWrapper<std::deque < JSON>> (nullptr);
	}

	/**
	 *Returns the whole json object as formatted string.
	 *@param depth number of indentation per level (defaults to 1)
	 *@param tab indentation character(s) (defaults to two spaces)
	 *@returns json object as formatted string.
	 */
	std::string dump(int depth = 1, std::string tab = "  ") const {
		switch (Type) {
			case Class::Null:
				return "null";
			case Class::Object: {
				std::string pad = "";
				std::string s = "{\n";
				bool skip = true;
				for ( int i = 0; i < depth; ++i, pad += tab );
				for ( auto &p: *Internal.Map ) {
					if ( !skip ) s += ",\n";
					s += ( pad + "\"" + p.first + "\" : " + p.second.dump(depth + 1, tab));
					skip = false;
				}

				s += ( "\n" + pad.erase(0, tab.size()) + "}" );
				return s;
			}
			case Class::Array: {
				std::string s = "[";
				bool skip = true;
				for ( auto &p: *Internal.List ) {
					if ( !skip ) s += ", ";
					s += p.dump(depth + 1, tab);
					skip = false;
				}

				s += "]";
				return s;
			}
			case Class::String:
				return "\"" + JSON::json_escape(*Internal.String) + "\"";
			case Class::Floating:
				return std::to_string(Internal.Float);
			case Class::Integral:
				return std::to_string(Internal.Int);
			case Class::Boolean:
				return Internal.Bool ? "true" : "false";
			default:
				return "";
		}
		return "";
	}

	/**
	 *Returns the whole json object as minified string.
	 *@returns json object as minified string.
	 */
	std::string dumpMinified() const {

		switch (Type) {
			case Class::Null:
				return "null";
			case Class::Object: {
				std::string s = "{";
				bool skip = true;
				for ( auto &p: *Internal.Map ) {
					if ( !skip ) s += ",";
					s += ( "\"" + p.first + "\":" + p.second.dumpMinified());
					skip = false;
				}
				s += "}";
				return s;
			}
			case Class::Array: {
				std::string s = "[";
				bool skip = true;
				for ( auto &p: *Internal.List ) {
					if ( !skip ) s += ",";
					s += p.dumpMinified();
					skip = false;
				}

				s += "]";
				return s;
			}
			case Class::String:
				return "\"" + JSON::json_escape(*Internal.String) + "\"";
			case Class::Floating:
				return std::to_string(Internal.Float);
			case Class::Integral:
				return std::to_string(Internal.Int);
			case Class::Boolean:
				return Internal.Bool ? "true" : "false";
			default:
				return "";
		}

		return "";
	}

	friend std::ostream &operator<<(std::ostream &, const JSON &);

	private:

	void SetType(Class type) {
		if ( type == Type )
			return;

		ClearInternal();

		switch (type) {
			case Class::Null:
				Internal.Map = nullptr;
				break;
			case Class::Object:
				Internal.Map = new std::map<std::string, JSON> ();
				break;
			case Class::Array:
				Internal.List = new std::deque<JSON> ();
				break;
			case Class::String:
				Internal.String = new std::string();
				break;
			case Class::Floating:
				Internal.Float = 0.0;
				break;
			case Class::Integral:
				Internal.Int = 0;
				break;
			case Class::Boolean:
				Internal.Bool = false;
				break;
		}

		Type = type;
	}

	/*beware: only call if YOU know that Internal is allocated. No checks performed here.
	    This function should be called in a constructed JSON just before you are going to
	    overwrite Internal...
	*/
	void ClearInternal() {
		switch (Type) {
			case Class::Object:
				delete Internal.Map;
				break;
			case Class::Array:
				delete Internal.List;
				break;
			case Class::String:
				delete Internal.String;
				break;
			default:;
		}
	}

	Class Type = Class::Null;

};

inline std::ostream &operator<<(std::ostream &os, const JSON &json) {
	os << json.dump();
	return os;
}

/**Overload to detect json::error as valid std::error_code. */
namespace std {
	template < > struct is_error_code_enum<JSON::error>: true_type {};
}
