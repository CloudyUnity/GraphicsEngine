#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* system = new SystemClass;

	if (system->Initialize())
		system->Run();
	else
	{
		system->Shutdown();
		delete system;
		system = 0;
		return 400;
	}		

	system->Shutdown();
	delete system;
	system = 0;
	return 0;
}
