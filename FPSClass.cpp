#include "FPSClass.h"

FpsClass::FpsClass()
{
}

FpsClass::FpsClass(const FpsClass& other)
{
}

FpsClass::~FpsClass()
{
}

void FpsClass::Initialize()
{
    m_fps = 0;
    m_count = 0;
	m_previousFPS = -1;

    m_startTime = timeGetTime();
}

void FpsClass::Frame()
{
    m_count++;

    if (timeGetTime() >= (m_startTime + 1000))
    {
        m_fps = m_count;
        m_count = 0;

        m_startTime = timeGetTime();
    }
}

int FpsClass::GetFps()
{
    return m_fps;
}

bool FpsClass::UpdateFPS(TextClass* text)
{
	char tempString[16], finalString[16];
	float red, green, blue;

	int fps = GetFps();

	if (m_previousFPS == fps)
		return true;

	m_previousFPS = fps;

	if (fps > 99999)
		fps = 99999;

	sprintf_s(tempString, "%d", fps);
	strcpy_s(finalString, "Fps: ");
	strcat_s(finalString, tempString);

	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	text->SetColor(red, green, blue);
	text->SetText(finalString);

	return text->UpdateText();
}