#include "Security.h"

#include "../Net/Socket.h"
#include "../Files/FileSystem.h"

#include <algorithm>
#include <iostream>
using namespace std;

std::string encryptionPassword = "FblIhn33T6pbapWU6u4IYX2moPOQ2ZFdM7qCzjxO3QwVOeKi7QkWIcGg2MYJfYVQMfaqKog0uAdgIqkz73v5abS6HxPegmlfLNqvdEJaNnpwLH94vLpSHVAuczPieSbO";

Security::Security(std::shared_ptr<INetManager> netManager)
        : netManager_(std::move(netManager))
{
	password_ = "Gs6F6yKucmA7OmAWSx3Gf2sIgXr0fD7IaZKebGR8JmfQEAUGvRhHHyWYohYZUCWfuYKVNlIlYUGBT8pLPkad9ig4ZT8noz633HZ8rBon9N1KLw1w9eeEnz7KEzrDDHdG";
}

bool Security::CheckIp()
{
	auto adapters = netManager_->GetAdapters();
	for (auto adapter : adapters)
	{
		IpAddress ip = adapter.GetNextIp();
		if (ip.Empty()) { return false; }
		CheckOther(ip.Get());
	}

	return true;
}

HostIp Security::CheckOther(const string & ip)
{
	if (InWhite(ip)) { return HostIp::WhiteList; }
	if (InBlack(ip)) { return HostIp::BlackList; }

	string myIp = netManager_->GetMyIp();

	if (netManager_->Send(ip, "check") &&
		netManager_->Send(ip, crypt_.open_key()))
	{
		string clientIp;
		string answer = netManager_->Receive(myIp, clientIp, 2);
		if (crypt_.decrypt(answer) == password_)
		{
			whiteList_.push_back(ip);
			cout << "Add " << ip << " to white list" << endl;
			return HostIp::WhiteList;
		}

		blackList_.push_back(ip);
		cout << "Add " << ip << " to black list" << endl;
		return HostIp::BlackList;
	}

	return HostIp::None;
}

void Security::ReceiveRequest()
{
	string myIp = netManager_->GetMyIp();
	string clientIp;
	string req = netManager_->Receive(myIp, clientIp, 2);
	if (req == "check")
	{
		vector<int> key = netManager_->ReceiveKey(myIp, clientIp, 2);
		netManager_->Send(clientIp, crypt_.encrypt(password_, key));
	}
}

bool Security::InWhite(const string & ip)
{
	return find(whiteList_.begin(), whiteList_.end(), ip) != whiteList_.end();
}

bool Security::InBlack(const string & ip)
{
	return find(blackList_.begin(), blackList_.end(), ip) != blackList_.end();
}

string Security::GetNextWhite()
{
	static int index = -1;
	if (whiteList_.empty()) { return ""; }
	++index;
	if (index == whiteList_.size()) {
		index = 0;
	}

	return whiteList_[index];
}
