#include "timerclass.h"


TimerClass::TimerClass()
{
}


TimerClass::TimerClass(const TimerClass& other)
{
}


TimerClass::~TimerClass()
{
}

bool TimerClass::Initialize()
{
    INT64 frequency;

    // Get the cycles per second speed for this system.
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    if (frequency == 0)
        return false;

    m_frequency = (float)frequency;

    // Get the initial start time.
    QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

    return true;
}

void TimerClass::Frame()
{
    INT64 currentTime;
    INT64 elapsedTicks;

    // Query the current time.
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime); // ?

    elapsedTicks = currentTime - m_startTime;
    m_frameTime = (float)elapsedTicks / m_frequency;

    m_startTime = currentTime;
}

float TimerClass::GetTime()
{
    return m_frameTime;
}