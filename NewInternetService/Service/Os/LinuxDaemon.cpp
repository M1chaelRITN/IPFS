#include "LinuxDaemon.h"

#include "../Service.h"

#ifndef _MSC_VER

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <syslog.h>
#include <cstring>

#include <thread>
using namespace std;

void LinuxDaemon::Start(const std::string & serviceName)
{
	Forking();
	ChangMask();
	CreateSID();
	ChangeDir();
	CloseStd();

	thread serviceThread = thread([]() {
		Service::runningService->Main();
	});
	serviceThread.join();
}

void LinuxDaemon::Forking()
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
	{
		exit(1);
	}
	if (pid > 0)
	{
		exit(0);
	}
}

void LinuxDaemon::ChangMask()
{
	umask(0);
}

void LinuxDaemon::CreateSID()
{
	pid_t sid;
	sid = setsid();
	if (sid < 0)
	{
		exit(1);
	}
}

void LinuxDaemon::ChangeDir()
{
	if ((chdir("/")) < 0)
	{
		exit(1);
	}
}

void LinuxDaemon::CloseStd()
{
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

void LinuxDaemon::Stop()
{

}

#endif
