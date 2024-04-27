#ifndef _LOG_H_
#define _LOG_H_

#include "IShutdown.h"
#include "Settings.h"
#include <string>
#include <fstream>
#include <stack>
#include <chrono>

using std::string;
using std::ofstream;
using std::stack;
using std::chrono::high_resolution_clock;

static ofstream* g_logOutStream;
static stack<high_resolution_clock::time_point> g_profileStack;

class LogClass : IShutdown
{	
public:
	static void Init();
	static void Log(string);
	static void LogStart(string);
	static void LogEnd(string);
	static void Shutdown();
};

#endif