#include "StringHelper.h"

#include <algorithm>

std::string vtos(const std::vector<uint8_t> & vec)
{
	std::string str;
	std::copy(vec.begin(), vec.end(), std::back_inserter(str));
	return str;
}

std::vector<uint8_t> stov(const std::string & str)
{
	std::vector<uint8_t> vec;
	std::copy(str.begin(), str.end(), std::back_inserter(vec));
	return vec;
}
