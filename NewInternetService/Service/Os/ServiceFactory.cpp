#include "ServiceFactory.h"

#include "WindowsService.h"
#include "LinuxDaemon.h"

OsService * ServiceFactory::Create()
{
#ifdef _MSC_VER
	return new WindowsService();
#else
	return new LinuxDaemon();
#endif
}
