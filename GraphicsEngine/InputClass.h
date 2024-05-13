#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include "IShutdown.h"

class InputClass : IShutdown
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

    bool Initialize(HINSTANCE, HWND, int, int);
    void Shutdown() override;
    bool Frame();

    bool GetKey(unsigned char keyCode);
    bool GetKeyDown(unsigned char keyCode);
    bool IsNumberPressed(int& outNumber);
    void GetMouseLocation(int&, int&);
    bool IsMousePressed();

private:
    bool ReadKeyboard();
    bool ReadMouse();
    void ProcessInput();

private:
    IDirectInput8* m_directInput;
    IDirectInputDevice8* m_keyboard;
    IDirectInputDevice8* m_mouse;
    unsigned char m_keyboardState[256];
    unsigned char m_keyboardStateLast[256];
    DIMOUSESTATE m_mouseState;
    int m_screenWidth, m_screenHeight, m_mouseX, m_mouseY;
    HWND m_hwnd;
};

#endif