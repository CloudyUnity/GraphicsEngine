#ifndef _FPSCLASS_H_
#define _FPSCLASS_H_

#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include "TextClass.h"

class FpsClass
{
public:
    FpsClass();
    FpsClass(const FpsClass&);
    ~FpsClass();

    void Initialize();
    void Frame();
    int GetFps();

    bool UpdateFPS(TextClass*);

private:
    int m_fps, m_count;
    unsigned long m_startTime;
    int m_previousFPS;
};

#endif