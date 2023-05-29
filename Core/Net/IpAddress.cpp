#include "IpAddress.h"

#include <sstream>
using namespace std;

IpAddress::IpAddress()
{
	for (int i = 0; i < 4; i++)
	{
		m_ipv4[i] = 0;
	}
}

IpAddress::IpAddress(const string & strIp)
{
	stringstream ssIp(strIp);
	string segment;

	for (int i = 0; i < 4 && getline(ssIp, segment, '.'); i++)
	{
		m_ipv4[i] = stoi(segment);
	}
}

IpAddress::IpAddress(int i1, int i2, int i3, int i4)
{
	m_ipv4[0] = i1;
	m_ipv4[1] = i2;
	m_ipv4[2] = i3;
	m_ipv4[3] = i4;
}

bool IpAddress::operator==(const IpAddress & other)
{
	return 	m_ipv4[0] == other.m_ipv4[0] &&
			m_ipv4[1] == other.m_ipv4[1] &&
			m_ipv4[2] == other.m_ipv4[2] &&
			m_ipv4[3] == other.m_ipv4[3];
}

bool IpAddress::operator!=(const IpAddress & other)
{
	return !operator==(other);
}

string IpAddress::Get()
{
	stringstream ss;
	for (int i = 0; i < 4; i++)
	{
		if (i != 0) { ss << "."; }
		ss << m_ipv4[i];
	}

	return ss.str();
}

bool IpAddress::Empty()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_ipv4[i] != 0) { return false; }
	}
	return true;
}
