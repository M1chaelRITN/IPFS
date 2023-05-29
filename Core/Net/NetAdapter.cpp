#include "NetAdapter.h"

NetAdapter::NetAdapter(const char * adapterName, uint8_t * mac, const char * ip, const char * mask, const char * gateway, bool dhcp)
{
	m_adapterName = adapterName;
	if (mac != nullptr) { std::copy(mac, mac + 8, m_mac.begin()); }
	m_ip = ip;
	m_mask = mask;
	m_gateway = gateway;
	m_dhcp = dhcp;
}

IpAddress NetAdapter::GetNextIp()
{
	static int jr = 1;

	IpAddress ip { m_ip };
	IpAddress mask { m_mask };
	IpAddress gw { m_gateway };

	int max_ip = (~mask.m_ipv4[0] << 24) |
		(~mask.m_ipv4[1] << 16) |
		(~mask.m_ipv4[2] << 8) |
		~mask.m_ipv4[3];

	if (jr == max_ip) { return IpAddress(); } // all subnet checked;

	IpAddress ia((ip.m_ipv4[0] & mask.m_ipv4[0]) | (jr >> 24),
				 (ip.m_ipv4[1] & mask.m_ipv4[1]) | ((jr >> 16) & 0xff),
				 (ip.m_ipv4[2] & mask.m_ipv4[2]) | ((jr >> 8) & 0xff),
				 (ip.m_ipv4[3] & mask.m_ipv4[3]) | (jr & 0xff));

	if (ia == gw || ia == ip) // gateway and self not check
	{
		return IpAddress();
	}
	jr++;

	return ia;
}
