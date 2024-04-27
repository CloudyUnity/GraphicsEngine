#include "LogClass.h"

void LogClass::Init()
{
	if (!LOGGING_ENABLED)
		return;

	g_logOutStream = new ofstream("../GraphicsEngine/Log.txt", ofstream::out);
}

void LogClass::Log(string str)
{
	if (!LOGGING_ENABLED)
		return;

	for (int i = 0; i < g_profileStack.size(); i++)
		(*g_logOutStream) << "=";

	(*g_logOutStream) << str << std::endl;
}

void LogClass::LogStart(string str)
{
	if (!LOGGING_ENABLED)
		return;

	for (int i = 0; i < g_profileStack.size(); i++)
		(*g_logOutStream) << "=";

	g_profileStack.push(high_resolution_clock::now());

	(*g_logOutStream) << str << std::endl;	
}

void LogClass::LogEnd(string str)
{
	if (!LOGGING_ENABLED)
		return;

	std::chrono::duration<float> time = high_resolution_clock::now() - g_profileStack.top();
	g_profileStack.pop();
	time *= 1000;

	for (int i = 0; i < g_profileStack.size(); i++)
		(*g_logOutStream) << "=";

	(*g_logOutStream) << str << " (" << time.count() << "ms)" << std::endl;
}

void LogClass::Shutdown()
{
	if (!LOGGING_ENABLED)
		return;

	g_logOutStream->close();
	delete g_logOutStream;
}
