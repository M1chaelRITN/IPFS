#pragma once

#include "NetAdapter.h"
#include "../Files/Buffer.h"

class INetManager
{
public:
	virtual std::string GetMyIp() = 0;

	virtual bool Send(const std::string & serverIp, const std::string & data) = 0;
	virtual bool Send(const std::string & serverIp, const std::vector<int> & data) = 0;
	virtual bool Send(const std::string & serverIp, std::shared_ptr<Buffer> buffer) = 0;
	virtual std::string Receive(const std::string & serverIp, std::string & clientIp, double sec) = 0;
	virtual std::vector<int> ReceiveKey(const std::string & serverIp, std::string & clientIp, double sec) = 0;

	virtual bool Upload(const std::string & fileName, const std::string & serverIp) = 0;
	virtual bool Download(const std::string & serverIp, const std::string & fileName) = 0;

	virtual std::vector<NetAdapter> GetAdapters() = 0;

};
