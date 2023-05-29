#pragma once

#include <string>

class OsService
{
public:
	virtual void Start(const std::string & serviceName) = 0;
	virtual void Stop() = 0;

	void Restart() {
		Stop();
		Start("");
	}

};
