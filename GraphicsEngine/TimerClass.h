#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_

#include <windows.h>

class TimerClass
{
public:
    TimerClass();
    TimerClass(const TimerClass&);
    ~TimerClass();

    bool Initialize();
    void Frame();

    float GetTime();

private:
    float m_frequency;
    INT64 m_startTime;
    float m_frameTime;
};

#endif