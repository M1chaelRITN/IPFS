#include "WindowsService.h"

#ifdef _MSC_VER

#include "../Service.h"

#include <thread>
using namespace std;

WindowsService * WindowsService::winService = nullptr;

void WINAPI ServiceCtrlHandler(unsigned long CtrlCode)
{
	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:

		if (WindowsService::winService->CurrentState() != SERVICE_RUNNING)
			break;

		WindowsService::winService->SetStatus(0, SERVICE_STOP_PENDING, "SERVICE_CONTROL_STOP Request", 4);
		Service::runningService->Exit();

		break;

	default:
		break;
	}
}

void WINAPI ServiceMain(unsigned long /*argc*/, wchar_t ** /*argv*/)
{
	WindowsService::winService->Register();
	WindowsService::winService->SetStatus(SERVICE_ACCEPT_STOP, SERVICE_RUNNING, "Performing Service Start Operations");

	thread serviceThread = thread([]() {
		Service::runningService->Main();
	});
	serviceThread.join();
}

void WindowsService::Start(const std::string & serviceName)
{
	m_serviceName = serviceName;
	winService = this;

	memset(&serviceStatus, 0, sizeof(&serviceStatus));

	print("Main: Entry");

	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ const_cast<char *>(serviceName.c_str()), (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{ nullptr, nullptr }
	};

	if (!StartServiceCtrlDispatcher(ServiceTable))
	{
		print("Main: StartServiceCtrlDispatcher returned error");
		//DWORD err = GetLastError();
		return;
	}

	print("Main: Exit");
}

void WindowsService::Stop()
{
	SetStatus(0, SERVICE_STOPPED, "Performing Cleanup Operations", 3);
	print("Exit");
}

void WindowsService::Register()
{
	print("Entry");

	statusHandle = RegisterServiceCtrlHandler(m_serviceName.c_str(), ServiceCtrlHandler);
	if (statusHandle == nullptr)
	{
		print("RegisterServiceCtrlHandler returned error");
		return;
	}

	SetStatus(0, SERVICE_START_PENDING, "Start pending");
}

void WindowsService::SetStatus(DWORD accepted, DWORD state, const std::string & message, DWORD checkPoint /*= 0*/)
{
	print(message);

	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwControlsAccepted = accepted;
	serviceStatus.dwCurrentState = state;
	serviceStatus.dwCheckPoint = checkPoint;

	if (!SetServiceStatus(statusHandle, &serviceStatus))
	{
		print("SetServiceStatus returned error");
	}
}

void WindowsService::print(const std::string & message)
{
	OutputDebugString((m_serviceName + ": " + message).c_str());
}

#endif
