#pragma once

#include "../Net/NetManager.h"
#include "../Encryption/Crypt.h"

#include <string>
#include <vector>

enum class HostIp
{
	None,
	WhiteList,
	BlackList
};

class Security
{
public:
	explicit Security(std::shared_ptr<INetManager> netManager);

	bool CheckIp();

	HostIp CheckOther(const std::string & ip);
	[[nodiscard]]
	std::shared_ptr<INetManager> GetNetManager() { return netManager_; }
	void ReceiveRequest();

	std::string GetNextWhite();

private:
	std::shared_ptr<INetManager> netManager_;
	std::string password_;
	Crypt crypt_;

	std::vector<std::string> whiteList_ = {}; // "123.123.123.123" «аполнить ip адресами,
	                                          // которые заранее известны.
	std::vector<std::string> blackList_;

	bool InWhite(const std::string & ip);
	bool InBlack(const std::string & ip);
};

extern std::string encryptionPassword;
