#pragma once

#include <string>
#include <array>

class IpAddress
{
public:
	IpAddress();
	explicit IpAddress(const std::string & strIp);
	IpAddress(int i1, int i2, int i3, int i4);

	std::string Get();

	bool Empty();
	bool operator==(const IpAddress & other);
	bool operator!=(const IpAddress & other);

//private:
	std::array<int, 4> m_ipv4;

};
