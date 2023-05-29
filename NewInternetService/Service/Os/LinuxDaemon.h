#pragma once

#include "OsService.h"

#ifndef _MSC_VER

class LinuxDaemon final : public OsService
{
public:
	void Start(const std::string & serviceName) override;
	void Stop() override;

private:
	void Forking();
	void ChangMask();
	void CreateSID();
	void ChangeDir();
	void CloseStd();

};

#endif
