#pragma once

#include "Socket.h"
#include "../Files/FileSystem.h"
#include "../Parameters/Parameters.h"
#include "IpAddress.h"
#include "NetAdapter.h"
#include "INetManager.h"

#include <string>
#include <vector>
#include <array>
#include <utility>

class NetManager final : public INetManager
{
public:
	NetManager();

	void Run(const Parameters & parameters);

	bool Send(const std::string & serverIp, const std::string & data) override;
	bool Send(const std::string & serverIp, const std::vector<int> & data) override;
	bool Send(const std::string & serverIp, std::shared_ptr<Buffer> buffer) override;
	std::string Receive(const std::string & serverIp, std::string & clientIp, double sec) override;
	std::vector<int> ReceiveKey(const std::string & serverIp, std::string & clientIp, double sec) override;

	bool Upload(const std::string & fileName, const std::string & serverIp) override;
	bool Download(const std::string & serverIp, const std::string & fileName) override;

	void PrintNetInfo();

	std::vector<NetAdapter> GetAdapters() override { return m_adapters; }
	std::string GetMyIp() override;

private:
	const u_short m_port = 4242;
	const int m_connections = SOMAXCONN;  // 5;

	std::vector<NetAdapter> m_adapters;

	bool LoadNetInfo();

};
