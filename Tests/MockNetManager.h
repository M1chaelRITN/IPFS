#pragma once

#include <vector>

#include "../Core/Net/INetManager.h"
#include "../Core/Net/NetAdapter.h"

class MockNetManager final : public INetManager
{
public:
	MockNetManager()
	{
		m_adapters.push_back(NetAdapter("loopback", nullptr, "127.0.0.1", "255.255.255.0", "0.0.0.0", true));
	}

	bool Send(const std::string & serverIp, const std::string & data) override
	{
		return true;
	}

	bool Send(const std::string & serverIp, const std::vector<int> & data) override
	{
		return true;
	}

	bool Send(const std::string & serverIp, std::shared_ptr<Buffer> buffer) override
	{
		return true;
	}

	std::string Receive(const std::string & serverIp, std::string & clientIp, double sec) override
	{
		return "password";
	}

	std::vector<int> ReceiveKey(const std::string & serverIp, std::string & clientIp, double sec) override
	{
		return {};
	}

	bool Upload(const std::string & fileName, const std::string & serverIp) override
	{
		return true;
	}

	bool Download(const std::string & serverIp, const std::string & fileName) override
	{
		return true;
	}

	std::vector<NetAdapter> GetAdapters() override { return m_adapters; }
	std::string GetMyIp() override
	{
		return m_adapters[0].Ip();
	}

private:
	std::vector<NetAdapter> m_adapters;

};
