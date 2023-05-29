#include "NetManager.h"

#ifdef _MSC_VER
#  include <iphlpapi.h>
#endif

#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <exception>
using namespace std;
using namespace chrono_literals;

NetManager::NetManager()
{
	LoadNetInfo();
}

void NetManager::Run(const Parameters & parameters)
{
	if (parameters.Method() == "upload")
	{
		bool uploaded = Upload(parameters.FileName(), parameters.RemoteIp());
		if (!uploaded)
		{
			throw runtime_error("Can't upload");
		}
	}
	else if (parameters.Method() == "download")
	{
		bool downloaded = Download(parameters.RemoteIp(), parameters.FileName());
		if (!downloaded)
		{
			throw runtime_error("Can't download");
		}
	}
}

bool NetManager::Send(const string & serverIp, const string & data)
{
	std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(data);
	return Send(serverIp, buffer);
}

bool NetManager::Send(const string & serverIp, const vector<int> & data)
{
	std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(data);
	return Send(serverIp, buffer);
}

bool NetManager::Send(const std::string & serverIp, std::shared_ptr<Buffer> buffer)
{
	SocketClient client(serverIp, m_port);

	thread sendThread([buffer, &client]() {
		client.SendBytes(buffer);
	});
	sendThread.join();

	return true;
}

bool NetManager::Upload(const string & fileName, const string & serverIp)
{
	auto fileContent = FileSystem::ReadFile(fileName);

	SocketClient client(serverIp, m_port);

	thread sendThread([fileContent, &client]() {
		client.SendBytes(fileContent);
	});
	sendThread.join();

	return true;
}

bool NetManager::Download(const string & serverIp, const string & fileName)
{
	SocketServer server(serverIp, m_port, m_connections);

	std::shared_ptr<Buffer> buffer {};

	Socket * clientSocket = server.Accept();
	thread answerThread([&buffer, &clientSocket]() {
		buffer = clientSocket->ReceiveBytes();
	});
	answerThread.join();
	// shutdown(m_socket, SD_BOTH);

	if (!buffer) { return false; }

	FileSystem::WriteFile(fileName, buffer);

	return true;
}

std::string NetManager::Receive(const string & serverIp, string & clientIp, double /*sec*/)
{
	SocketServer server(serverIp, m_port, m_connections);

	std::shared_ptr<Buffer> buffer {};

	Socket * clientSocket = server.Accept();
	clientIp = server.GetAcceptedIp();
	thread answerThread([&buffer, &clientSocket]() {
		buffer = clientSocket->ReceiveBytes();
	});
	answerThread.join();
	// shutdown(m_socket, SD_BOTH);

	if (!buffer) { return ""; }

	std::string str = std::string(buffer->Bytes(), buffer->Size());

	return str;
}

std::vector<int> NetManager::ReceiveKey(const string & serverIp, string & clientIp, double /*sec*/)
{
	SocketServer server(serverIp, m_port, m_connections);

	std::shared_ptr<Buffer> buffer {};

	Socket * clientSocket = server.Accept();
	clientIp = server.GetAcceptedIp();
	thread answerThread([&buffer, &clientSocket]() {
		buffer = clientSocket->ReceiveBytes();
	});
	answerThread.join();
	// shutdown(m_socket, SD_BOTH);

	if (!buffer) { return {}; }

	int * data = (int *)buffer->Bytes();
	size_t size = buffer->Size() / sizeof(int);

	vector<int> vec;
	copy(data, data + size, back_inserter(vec));

	return vec;
}

bool NetManager::LoadNetInfo()
{
#ifdef _MSC_VER
	PIP_ADAPTER_INFO AdapterInfo = nullptr;
	ULONG sz = sizeof(IP_ADAPTER_INFO);
	DWORD dwStatus;
	AdapterInfo = (PIP_ADAPTER_INFO)malloc(sz);
	if (AdapterInfo == nullptr) { return false; }
	dwStatus = GetAdaptersInfo(AdapterInfo, &sz);
	if (dwStatus == ERROR_BUFFER_OVERFLOW)
	{
		free(AdapterInfo);
		AdapterInfo = (PIP_ADAPTER_INFO)malloc(sz);
		dwStatus = GetAdaptersInfo(AdapterInfo, &sz);
		if (AdapterInfo == NULL) { return false; }
	}
	if (dwStatus != NO_ERROR)
	{
		if (AdapterInfo != nullptr) { free(AdapterInfo); }
		return false;
	}

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	while (pAdapterInfo)
	{
		std::string ip = pAdapterInfo->IpAddressList.IpAddress.String;
		if (ip != "0.0.0.0")
		{
			m_adapters.push_back(NetAdapter(
				pAdapterInfo->AdapterName,
				pAdapterInfo->Address,
				pAdapterInfo->IpAddressList.IpAddress.String,
				pAdapterInfo->IpAddressList.IpMask.String,
				pAdapterInfo->GatewayList.IpAddress.String,
				!!pAdapterInfo->DhcpEnabled
			));
		}
		pAdapterInfo = pAdapterInfo->Next;
	};

	free(AdapterInfo);
	if (pAdapterInfo == nullptr) { return false; }
#endif

	return true;
}

void NetManager::PrintNetInfo()
{
	for (const auto & adapter : m_adapters)
	{
		cout << "adapter: " << adapter.Name() << endl;
		cout << "   ip: " << adapter.Ip() << endl;
		cout << "   mask: " << adapter.Mask() << endl;
		cout << "   gateway: " << adapter.Gateway() << endl;
		cout << "   dhcp: " << (adapter.Dhcp() ? "yes" : "no") << endl << endl;
	}
}

string NetManager::GetMyIp()
{
	return m_adapters.empty() ? "" : m_adapters[0].Ip();
}
