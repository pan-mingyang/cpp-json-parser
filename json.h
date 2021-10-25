#pragma once
#ifndef _JSON_H_
#define _JSON_H_

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <utility>
#include <tuple>

namespace my_json {

using str_iter = std::string::iterator;

enum class JSONDataType {
	Int, Double, String, Bool, List, Object, Null
};

class JSON
{
public:

	using object_type = std::vector<std::pair<std::string, JSON>>;
	using list_type = std::vector<JSON>;

	JSON();
	JSON(int);
	JSON(double);
	JSON(bool);
	JSON(const std::string&);
	JSON(const std::vector<JSON>&);
	JSON(const object_type&);
	JSON(std::string&& );
	JSON(std::vector<JSON>&& );
	JSON(object_type&& );
	JSON(const JSON& );
	JSON(JSON&& );
	//JSON(JSON&& ) = delete;

	~JSON();

	void print_str(std::ostream& os, const std::string& str) const;

	void print_w(std::ostream& os, int indent) const;

	void print(std::ostream& os, size_t indent = 2, size_t k = 0, bool first = true) const;

	std::string get_str() const;

	JSON& operator=(const JSON& json);

	JSON& operator=(JSON&& json) noexcept;

private:
	JSONDataType _type;
	//union
	//{
	int _int_val;
	double _double_val;
	bool _bool_val;
	std::string _string_val;
	list_type _list;
	object_type _object;
	//};
};


} // namespace my_json

#endif

