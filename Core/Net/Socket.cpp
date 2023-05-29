#include "Socket.h"

#include <cstring>

#include <iostream>
#include <exception>
#include <vector>
using namespace std;

int Socket::m_nofSockets = 0;

void Socket::Start()
{
	if (!m_nofSockets)
	{
#ifdef _MSC_VER
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 2), &info))
		{
			throw std::exception("Could not start WSA");
		}
#endif
	}
	++m_nofSockets;
}

void Socket::End()
{
#ifdef _MSC_VER
	WSACleanup();
#endif
}

Socket::Socket() : m_socket(0)
{
	Start();
	// UDP: use SOCK_DGRAM instead of SOCK_STREAM
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		throw runtime_error("INVALID_SOCKET");
	}

	m_refCounter = new int(1);
}

Socket::Socket(SOCKET s) : m_socket(s)
{
	Start();
	m_refCounter = new int(1);
};

Socket::~Socket()
{
	if (!--(*m_refCounter))
	{
		Close();
		delete m_refCounter;
	}

	--m_nofSockets;
	if (!m_nofSockets) End();
}

Socket::Socket(const Socket & o)
{
	m_refCounter = o.m_refCounter;
	(*m_refCounter)++;
	m_socket = o.m_socket;

	m_nofSockets++;
}

Socket & Socket::operator=(const Socket & o)
{
	(*o.m_refCounter)++;

	m_refCounter = o.m_refCounter;
	m_socket = o.m_socket;

	m_nofSockets++;

	return *this;
}

void Socket::Close()
{
#ifdef _MSC_VER
	closesocket(m_socket);
#else
	close(m_socket);
#endif
}

std::shared_ptr<Buffer> Socket::ReceiveBytes()
{
	auto buffer = std::make_shared<Buffer>();
	vector<char> buff(BUFF_SIZE);

	int packet_size = 0;
	while (true)
	{
		packet_size = recv(m_socket, buff.data(), static_cast<int>(buff.size()), 0);
		if (packet_size <= 0)
			break;

		buffer->Add(buff, packet_size);
	}

	return buffer;
}

void Socket::SendBytes(const string & s)
{
	send(m_socket, s.c_str(), static_cast<int>(s.length()), 0);
}

void Socket::SendBytes(std::shared_ptr<Buffer> buffer)
{
	int packet_size = send(m_socket, (char *)buffer->Bytes(), static_cast<int>(buffer->Size()), 0);
	if (packet_size == -1)
	{
		Close();
		End();
		throw runtime_error("Can't send message to Client.");
	}
}

SocketServer::SocketServer(const string & host, u_short port, int connections)
{
	in_addr ip_to_num;
	int erStat = inet_pton(AF_INET, host.c_str(), &ip_to_num);
	if (erStat <= 0)
	{
		Close();
		End();
		throw runtime_error("Error in IP translation to special numeric format");
	}

	sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr = ip_to_num;
	serverAddress.sin_port = htons(port);

	erStat = bind(m_socket, (sockaddr *)&serverAddress, sizeof(serverAddress));
	if (erStat != 0)
	{
		Close();
		End();
		throw runtime_error("Error Socket binding to server info.");
	}

	erStat = listen(m_socket, connections);
	if (erStat != 0)
	{
		Close();
		End();
		throw runtime_error("Can't start to listen to.");
	}
}

Socket * SocketServer::Accept()
{
	sockaddr_in clientAddress;
	memset(&clientAddress, 0, sizeof(clientAddress));
	socklen_t clientAddressSize = sizeof(clientAddress);

	SOCKET connectedClient = accept(m_socket, (sockaddr *)&clientAddress, &clientAddressSize);
	if (connectedClient == INVALID_SOCKET)
	{
		Close();
		End();
#ifdef _MSC_VER
		closesocket(connectedClient);
#else
		close(connectedClient);
#endif
		throw runtime_error("Client detected, but can't connect to a client.");
	}

	struct in_addr ipAddr = clientAddress.sin_addr;
	char strIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ipAddr, strIp, INET_ADDRSTRLEN);
	m_acceptedIp = strIp;

	// ipv6
	// struct sockaddr_in6 * pV6Addr = (struct sockaddr_in6 *)&client_addr;
	// struct in6_addr ipAddr = pV6Addr->sin6_addr;
	// char str[INET6_ADDRSTRLEN];
	// inet_ntop(AF_INET6, &ipAddr, str, INET6_ADDRSTRLEN);

	return new Socket(connectedClient);
}

SocketClient::SocketClient(const string & host, u_short port) : Socket()
{
	in_addr ip_to_num;
	int erStat = inet_pton(AF_INET, host.c_str(), &ip_to_num);
	if (erStat <= 0)
	{
		Close();
		End();
		throw runtime_error("Error in IP translation to special numeric format");
	}

	sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr = ip_to_num;
	serverAddress.sin_port = htons(port);

	erStat = connect(m_socket, (sockaddr *)&serverAddress, sizeof(serverAddress));
	if (erStat != 0)
	{
		Close();
		End();
		throw runtime_error("Connection to Server is FAILED.");
	}
}

