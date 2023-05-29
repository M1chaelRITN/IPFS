#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>

template<typename T>
std::string to_string(T number) {
	std::ostringstream os;
	os << number;
	return os.str();
}

extern std::string vtos(const std::vector<uint8_t> & vec);

extern std::vector<uint8_t> stov(const std::string & str);
