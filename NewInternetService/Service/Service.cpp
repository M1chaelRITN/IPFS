#include "Service.h"

#include "Os/ServiceFactory.h"

#include <thread>
using namespace std;

string Service::m_serviceName;
Service * Service::runningService = nullptr;

Service::Service(const string & serviceName)
       : exitThread(false)
{
    m_serviceName = serviceName;
    osService = ServiceFactory::Create();
}

int Service::Run()
{
    if (runningService != nullptr)
    {
        throw runtime_error("Service already running");
    }
    runningService = this;

    osService->Start(m_serviceName);
    osService->Stop();

    return 0;
}
