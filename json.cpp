#include "json.h"
#include <string>
#include <iostream>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <cmath>

namespace my_json {

// constructors
JSON::JSON()
	:_type(JSONDataType::Null), _int_val(0) {}

JSON::JSON(int int_val) 
	: _type(JSONDataType::Int), _int_val(int_val) {}

JSON::JSON(double double_val) 
	: _type(JSONDataType::Double), _double_val(double_val) {}

JSON::JSON(bool bool_val)
	: _type(JSONDataType::Bool), _bool_val(bool_val) {}

JSON::JSON(const std::string& string_val)
	: _type(JSONDataType::String), _string_val(string_val) {}

JSON::JSON(const std::vector<JSON>& list)
	: _type(JSONDataType::List), _list(list) {}

JSON::JSON(const object_type& object)
	: _type(JSONDataType::Object), _object(object) {}

JSON::JSON(std::string&& string_val)
	: _type(JSONDataType::String), _string_val(std::move(string_val)) {}

JSON::JSON(std::vector<JSON>&& list)
	: _type(JSONDataType::List), _list(std::move(list)) {}

JSON::JSON(object_type&& object)
	: _type(JSONDataType::Object), _object(std::move(object)) {}



JSON::JSON(const JSON& json)
	: _type(json._type)
{
	switch (json._type)
	{
	case JSONDataType::Int:
		_int_val = json._int_val; break;
	case JSONDataType::Double:
		_double_val = json._double_val; break;
	case JSONDataType::Bool:
		_double_val = json._bool_val; break;
	case JSONDataType::String:
		_string_val = json._string_val; break;
	case JSONDataType::List:
		_list = list_type(json._list); break;
	case JSONDataType::Object:
		_object = object_type(json._object); break;
	default:
		break;
	}
}

JSON::JSON(JSON&& json)
	: _type(json._type)
{
	if (this == &json) return;

	switch (json._type)
	{
	case JSONDataType::Int:
		_int_val = json._int_val;
		break;
	case JSONDataType::Double:
		_double_val = json._double_val;
		break;
	case JSONDataType::Bool:
		_double_val = json._bool_val;
		break;
	case JSONDataType::String:
		_string_val = std::string(std::move(json._string_val));
		break;
	case JSONDataType::List:
		_list = list_type(std::move(json._list));
		//json._list.clear();
		break;
	case JSONDataType::Object:
		_object = object_type(std::move(json._object));
		//json._object.clear();
		break;
	default:
		break;
	}
}

JSON::~JSON() {}

void JSON::print_str(std::ostream& os, const std::string& str) const
{
	os << '\"';
	for (auto ch : str) {
		switch (ch) {
		case '\n':
			os << "\\n";   break;
		case '\t':
			os << "\\t";   break;
		case '\a':
			os << "\\a";   break;
		case '\b':
			os << "\\b";   break;
		case '\r':
			os << "\\r";   break;
		case '\"':
			os << "\\\"";  break;
		default:
			os << ch;
		}
	}
	os << '\"';
}

void JSON::print_w(std::ostream& os, int indent) const
{
	while (--indent >= 0) {
		os << " ";
	}
}

void JSON::print(std::ostream& os, size_t indent, size_t k, bool first) const
{
	if (first) {
		print_w(os, indent * k);
	}

	switch (_type)
	{
	case JSONDataType::Null:
		os << "null";
		break;

	case JSONDataType::Int:
		os << _int_val;
		break;

	case JSONDataType::Double:
		os << _double_val;
		break;

	case JSONDataType::Bool:
		os << (_bool_val ? "true" : "false");
		break;

	case JSONDataType::String:		
		print_str(os, _string_val);
		break;

	case JSONDataType::List:
		os << "[\n";
		for (auto p = _list.begin(); p != _list.end(); ++p) {
			p->print(os, indent, k + 1);
			if (p + 1 != _list.end()) {
				os << ",\n";
			} else {
				os << "\n";
			}
		}		
		print_w(os, indent * k);
		os << "]";
		break;

	case JSONDataType::Object:
		os << "{\n";
		for (auto p = _object.begin(); p != _object.end(); ++p) {
			print_w(os, indent * (k + 1));
			print_str(os, p->first);
			os << ": ";
			p->second.print(os, indent, k + 1, false);
			if (p + 1 != _object.end()) {
				os << ",\n";
			} else {
				os << "\n";
			}			
		}
		print_w(os, indent * k);
		os << "}";
		break;
	default:
		break;
	}

}

std::string JSON::get_str() const
{
	std::ostringstream os;
	print(os);
	return os.str();
}

JSON& JSON::operator=(const JSON& json)
{

	_type = json._type;
	switch (json._type)
	{
	case JSONDataType::Int:
		_int_val = json._int_val; break;
	case JSONDataType::Double:
		_double_val = json._double_val; break;
	case JSONDataType::Bool:
		_double_val = json._bool_val; break;
	case JSONDataType::String:
		_string_val = json._string_val; break;
	case JSONDataType::List:
		_list = json._list; break;
	case JSONDataType::Object:
		_object = json._object; break;
	default:
		break;
	}

	return *this;
}


JSON& JSON::operator=(JSON&& json) noexcept
{
	if (this == &json) return *this;

	_type = json._type;

	switch (json._type)
	{
	case JSONDataType::Int:
		_int_val = json._int_val; break;
	case JSONDataType::Double:
		_double_val = json._double_val; break;
	case JSONDataType::Bool:
		_double_val = json._bool_val; break;
	case JSONDataType::String:
		_string_val = std::string(std::move(json._string_val));
		break;
	case JSONDataType::List:
		_list = list_type(std::move(json._list));
		//json._list.clear();
		break;
	case JSONDataType::Object:
		_object = object_type(std::move(json._object));
		//json._object.clear();
		break;
	default:
		break;
	}

	return *this;
}



}
