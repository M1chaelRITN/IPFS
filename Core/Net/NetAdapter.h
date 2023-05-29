#pragma once

#include <string>
#include <array>
#include <utility>

#include "IpAddress.h"

class NetAdapter
{
public:
	NetAdapter(const char * adapterName, uint8_t * mac, const char * ip, const char * mask, const char * gateway, bool dhcp);

	IpAddress GetNextIp();

	[[nodiscard]] std::string Name() const { return m_adapterName; }
	[[nodiscard]] std::string Ip() const { return m_ip; }
	[[nodiscard]] std::string Mask() const { return m_mask; }
	[[nodiscard]] std::string Gateway() const { return m_gateway; }
	[[nodiscard]] bool Dhcp() const { return m_dhcp; }

private:
	std::string m_adapterName;
	std::array<uint8_t, 8> m_mac;
	std::string m_ip;
	std::string m_mask;
	std::string m_gateway;
	bool m_dhcp;

};
