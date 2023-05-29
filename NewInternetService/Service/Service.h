#pragma once

#include <string>
#include <atomic>

#include "../../Core/framework.h"
#include "Os/OsService.h"

class Service
{
public:
	explicit Service(const std::string & serviceName);
	virtual ~Service() = default;

	int Run();

	virtual void Main() = 0;

	void Exit() {
		exitThread = true;
	}

	static Service * runningService;

protected:
	std::atomic_bool exitThread;

private:
	static std::string m_serviceName;
	OsService * osService;

};
