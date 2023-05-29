#include "NewInternetService.h"

#include <vector>
#include <thread>
#include <chrono>
using namespace std;
using namespace chrono_literals;

NewInternetService::NewInternetService()
                  : Service("New Internet Service")
{
	m_netManager = make_shared<NetManager>();
	m_security = make_unique<Security>(m_netManager);
}

void NewInternetService::Main()
{
	while (!exitThread)
	{
		CheckTiers();
		m_security->CheckIp();
		m_security->ReceiveRequest();

		string myIp = m_netManager->GetMyIp();
		string clientIp;
		string command = m_netManager->Receive(myIp, clientIp, 1.0);
		if (!command.empty())
		{
			if (command.substr(0, 8) == "download")
			{
				string sFileId = command.substr(9);
				size_t pos = sFileId.find_first_of(' ');
				if (pos == string::npos)
				{
					string sId = sFileId.substr(pos + 1);
					sFileId = sFileId.substr(0, pos);
					
					fs::path filePath = FileSystem::GetMetaFolder();
					filePath /= to_string(sFileId);
					filePath /= to_string(sId);
					filePath /= "_c";

					string fileName = filePath.generic_string();

					if (FileSystem::Exists(fileName))
					{
						m_netManager->Upload(fileName, clientIp);
					}
				}
			}
			if (command.substr(0, 8) == "upload")
			{
				string sFileId = command.substr(9);
				size_t pos = sFileId.find_first_of(' ');
				if (pos == string::npos)
				{
					string sId = sFileId.substr(pos + 1);
					sFileId = sFileId.substr(0, pos);

					fs::path filePath = FileSystem::GetMetaFolder();
					filePath /= to_string(sFileId);
					filePath /= to_string(sId);
					filePath /= "_c";

					string fileName = filePath.generic_string();

					m_netManager->Download(fileName, clientIp);
				}
			}
		}

		std::this_thread::sleep_for(100ms);
	}
}

void NewInternetService::CheckTiers()
{
	vector<Unit> files = m_repository.List();

	for (auto file : files)
	{
		file.CheckTier();
	}

	for (auto file : files)
	{
		// do one file per time
		if (!file.TierCompliance())
		{
			file.CompleteTier();
			break;
		}
	}
}
