#pragma once

#ifdef _MSC_VER
#  include <WinSock2.h>
#  include <ws2tcpip.h>
#else
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <netdb.h>
#  include <sys/socket.h>
#  include <unistd.h>
#  include <sys/time.h>

using SOCKET = int;
const int INVALID_SOCKET = -1;
#endif

#include <string>
#include <memory>

#include "../Files/Buffer.h"

class Socket
{
public:

	virtual ~Socket();
	Socket(const Socket &);
	Socket & operator=(const Socket &);

	void Close();

	std::shared_ptr<Buffer> ReceiveBytes();
	void SendBytes(const std::string &);
	void SendBytes(std::shared_ptr<Buffer> buffer);

protected:
	friend class SocketServer;
	friend class SocketSelect;

	explicit Socket(SOCKET s);
	Socket();

	static void End();

	SOCKET m_socket;

	int * m_refCounter;

private:
	static void Start();

	static int  m_nofSockets;
	const short BUFF_SIZE = 1024;

};

class SocketClient : public Socket
{
public:
	SocketClient(const std::string & host, u_short port);

};

class SocketServer : public Socket
{
public:
	SocketServer(const std::string & host, u_short port, int connections);

	Socket * Accept();
	[[nodiscard]] std::string GetAcceptedIp() const { return m_acceptedIp; }

private:
	std::string m_acceptedIp;

};
