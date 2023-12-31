#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* system = new SystemClass;

	if (system->Initialize())
		system->Run();
	else
		throw new std::exception("Failed to initliaze system");

	system->Shutdown();
	delete system;
	system = 0;

	return 0;
}
