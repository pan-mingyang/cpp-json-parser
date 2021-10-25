#include "json_parser.h"
#include <iostream>
#include <sstream>

namespace my_json {


JSONParser::JSONParser() 
	: _str(""), _begin(_str.begin()), _end(_str.end()), 
	_error(false), _line_no(1), _file(false) {}

JSONParser::JSONParser(const std::string& str, bool file)
	: _error(false), _line_no(1), _file(file)
{
	if (not file) {
		_str = str;
		_begin = _str.begin();
		_end = _str.end();
	} else {
		_ifs.open(str.c_str(), std::ios_base::in);
		if (not _ifs) {
			_str = "";
		} else {
			if (not std::getline(_ifs, _str)) _str = "";
		}
		_begin = _str.begin();
		_end = _str.end();
	}
}

JSONParser::~JSONParser() {
	if (_ifs.is_open()) {
		_ifs.close();
	}
}



void JSONParser::update(const std::string& str)
{
	_str = std::string(str);
	_begin = _str.begin();
	_end = _str.end();
}

JSON JSONParser::parse_json()
{
	skip_whitspaces();
	JSON json = parse_multitype_value();
	skip_whitspaces();
	if (_begin != _end) {
		_error = true;
	}
	if (_error) {
		std::cout << "ERROR: " << _line_no;
	}
	return json;
}

std::tuple<JSONDataType, double, int> 
JSONParser::parse_number()
{
	int i_val = 0;
	double d_val = 0;
	int sign = 1;
	int e_sign = 0;
	int e = 0;
	int state = 0;
	double ratio = 0.1;
	auto type = JSONDataType::Int;
	bool start = false;

	while (_begin != _end and state != 255) {

		switch (state)
		{
		case 0:  // 正负号
			if (*_begin == '+') {
				++_begin;
			} else if (*_begin == '-') {
				sign = -1;
				++_begin;
			} else if (*_begin < '0' or *_begin > '9') {
				state = -1;
			}
			state = 1;
			break;
		case 1:  // 整数 & 浮点数的整数部分
			if (*_begin >= '0' and *_begin <= '9') {
				i_val *= 10;
				i_val += *_begin - '0';
				++_begin;
				start = true;
			} else if (*_begin == '.') {
				d_val = static_cast<double>(i_val);
				type = JSONDataType::Double;
				state = 2;
				++_begin;
			} else if (start and (*_begin == 'e' or *_begin == 'E')) {
				d_val = static_cast<double>(i_val);
				state = 3;
				type = JSONDataType::Double;
				++_begin;
			} else if (start) {
				state = 255;
			} else {
				state = -1;
			}
			break;
		case 2:  // 小数部分
			if (*_begin >= '0' and *_begin <= '9') {
				d_val += ratio * (*_begin - '0');
				ratio /= 10;
				++_begin;
			} else if (*_begin == 'e' || *_begin == 'E') {
				state = 3; ++_begin;
			} else {
				state = 255;
			}
			break;
		case 3:  // 指数部分
			if (*_begin == '+') {
				e_sign = 1; ++_begin;
			} else if (*_begin == '-') {
				e_sign = -1; ++_begin;
			} else if (*_begin >= '0' and *_begin <= '9') {
				if (e_sign == 0) {
					e_sign = 1;
				}
				e = e * 10 + (*_begin - '0');
				++_begin;
			} else if (e_sign == 0) {
				state = -1;
			} else {
				state = 1;
			}
			break;
		default:
			break;
		}

		if (state == -1) {
			_error = true;
			return std::make_tuple(JSONDataType::Null, 0.0, 0);
		}
	}

	if (e_sign != 0) {
		d_val = d_val * pow(10, e_sign * e);
	}

	return std::make_tuple(type, d_val * sign, i_val * sign);
}


std::string JSONParser::parse_string()
{
	std::ostringstream oss;
	bool vslash = false;
	bool end = false;
	++_begin;
	while (_begin != _end) {
		if (not vslash and *_begin == '\\') {
			vslash = true;
			++_begin;
			continue;
		}
		if (*_begin == '\"' and not vslash) {
			++_begin;
			end = true;
			break;
		}
		if (vslash) {
			switch (*_begin) {
			case 'a':
				oss << '\a';   break;
			case 'b':
				oss << '\b';   break;
			case 'n':
				oss << '\n';   break;
			case 't':
				oss << '\t';   break;
			case 'r':
				oss << '\r';   break;
			default:
				oss << *_begin; break;
			}
			vslash = false;
		} else oss << *_begin;
		++_begin;
	}
	if (not end) _error = true;
	return oss.str();
}

JSONParser::list_type 
JSONParser::parse_list()
{
	++_begin;
	list_type list;
	while (_begin != _end) {
		skip_whitspaces();
		JSON elem = parse_multitype_value();		
		if (_error) break;
		list.push_back(std::move(elem));
		skip_whitspaces();
		if (*_begin == ',') {
			++_begin;
			continue;
		} else if (*_begin == ']') {
			++_begin;
			break;
		} else {
			_error = true;
			break;
		}
	}

	return list;
}

JSONParser::object_type 
JSONParser::parse_object()
{
	++_begin;
	object_type object;
	int state = 1;
	while (_begin != _end) {
		skip_whitspaces();
		std::string key;

		if (*_begin == '\"') {
			key = parse_string();
			if (_error) break;
		} else if (*_begin == '}') {
			++_begin;
			break;
		} else {
			_error = true;
			break;
		}
		skip_whitspaces();

		if (*_begin != ':') {
			_error = true;
			break;
		}
		++_begin;
		skip_whitspaces();

		JSON value = parse_multitype_value();
		if (_error) break;

		object.push_back(std::make_pair(std::string(std::move(key)), std::move(value)));
		skip_whitspaces();

		if (*_begin == ',') {
			++_begin;
			continue;
		} else if (*_begin == '}') {
			++_begin;
			break;
		} else {
			_error = true;
			break;
		}
	}

	return object;
}

JSON JSONParser::parse_multitype_value()
{
	JSON elem;

	if (*_begin == '\"') {
		elem = JSON(parse_string());

	} else if (*_begin == '+' or *_begin == '-'
			   or (*_begin >= '0' and *_begin <= '9')
			   or *_begin == '.') {
		auto tmp = parse_number();
		if (std::get<0>(tmp) == JSONDataType::Double) {
			elem = JSON(std::get<double>(tmp));
		} else if (std::get<0>(tmp) == JSONDataType::Int) {
			elem = JSON(std::get<int>(tmp));
		} else {
			elem = JSON();
			_error = true;
		}
	} else if (*_begin == '[') {
		elem = JSON(parse_list());
	} else if (*_begin == '{') {
		elem = JSON(parse_object());
	} else if (*_begin == 'n') {
		if (_str.substr(_begin - _str.begin(), 4) == "null") {
			elem = JSON();
			_begin += 4;
		} else {
			_error = true;
		}
	} else if (*_begin == 'f') {
		if (_str.substr(_begin - _str.begin(), 5) == "false") {
			elem = JSON(false);
			_begin += 5;
		} else {
			_error = true;
		}
	} else if (*_begin == 't') {
		if (_str.substr(_begin - _str.begin(), 4) == "true") {
			elem = JSON(true);
			_begin += 4;
		} else {
			_error = true;
		}
	} else {
		_error = true;
	}

	return elem;
}

void JSONParser::skip_whitspaces()
{
	while (_begin != _end and (*_begin == ' ' or *_begin == '\t' or
							   *_begin == '\n' or *_begin == '\r')) {
		if (*_begin == '\n') {
			++_line_no;
		}
		++_begin;
	}
}

}