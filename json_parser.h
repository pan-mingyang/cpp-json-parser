#pragma once
#ifndef _JSON_PARSER_
#define _JSON_PARSER_
#include <string>
#include <fstream>
#include "json.h"

namespace my_json {

class JSONParser {

public:

	using list_type = JSON::list_type;
	using object_type = JSON::object_type;

	JSONParser();
	JSONParser(const std::string& str, bool file = false);

	~JSONParser();

	void update(const std::string& str);

	JSON parse_json();

	std::tuple<JSONDataType, double, int> parse_number();

	std::string parse_string();

	list_type parse_list();

	object_type parse_object();

	JSON parse_multitype_value();

	void skip_whitspaces();

private:
	std::string _str;
	str_iter _begin;
	str_iter _end;

	bool _error;
	size_t _line_no;
	std::ifstream _ifs;
	bool _file;
	

};


}
#endif // !_JSON_PARSER_
