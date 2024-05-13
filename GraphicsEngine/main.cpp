#include "SystemClass.h"
#include "LogClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	LogClass::Init();
	LogClass::Log("Logging Initialised");

	LogClass::LogStart("Initialising System");

	SystemClass* system = new SystemClass;

	if (system->Initialize())
	{
		LogClass::LogEnd("System Initialised");
		LogClass::Log("\nRunning engine");
		system->Run();
	}		
	else
	{
		LogClass::LogEnd("Error initalising system, exiting with code 400");

		system->Shutdown();
		delete system;
		system = 0;
		LogClass::Shutdown();	
		return 400;
	}			

	LogClass::LogStart("\nEngine exited. Shutting down");

	system->Shutdown();
	delete system;
	system = 0;

	LogClass::LogEnd("Shutdown complete");

	LogClass::Log("Program exiting with code 0");

	LogClass::Shutdown();

	return 0;
}
