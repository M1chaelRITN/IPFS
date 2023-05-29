#pragma once

#ifdef _MSC_VER

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

#include "OsService.h"

class WindowsService final : public OsService
{
public:
	void Start(const std::string & serviceName) override;
	void Stop() override;

	static WindowsService * winService;
	
	void Register();
	void SetStatus(DWORD accepted, DWORD state, const std::string & message, DWORD checkPoint = 0);
	DWORD CurrentState() const { return serviceStatus.dwCurrentState; }

private:
	std::string m_serviceName;
	SERVICE_STATUS_HANDLE statusHandle;
	SERVICE_STATUS serviceStatus;

	void print(const std::string & message);

};

#endif
