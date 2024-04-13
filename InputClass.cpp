#include "InputClass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_hwnd = hwnd;	

	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
		return false;

	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
		return false; 

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return false;

	result = m_keyboard->Acquire();
	if (FAILED(result))
		return false;

	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
		return false;

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
		return false;

	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	result = m_mouse->Acquire();
	if (FAILED(result))
		return false;

	return true;
}

void InputClass::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool InputClass::Frame()
{
	bool result;

	for (int i = 0; i < 256; i++)
		m_keyboardStateLast[i] = m_keyboardState[i];

	result = ReadKeyboard();
	if (!result)
		return false;

	result = ReadMouse();
	if (!result)
		return false;

	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (!FAILED(result))
		return true;
	
	// If the keyboard lost focus or was not acquired then try to get control back.
	if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
	{
		m_keyboard->Acquire();
		return true;
	}
	
	return false;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (!FAILED(result))
		return true;
	
	// If the mouse lost focus or was not acquired then try to get control back.
	if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
	{
		m_mouse->Acquire();
		return true;
	}

	return false;
}

void InputClass::ProcessInput()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(m_hwnd, &cursorPos);

	m_mouseX = cursorPos.x;
	m_mouseY = cursorPos.y;
}

bool InputClass::GetKey(unsigned char keyCode)
{
	return m_keyboardState[keyCode] & 0x80;
}

bool InputClass::GetKeyDown(unsigned char keyCode)
{
	return (m_keyboardStateLast[keyCode] & 0x80) == 0 && (m_keyboardState[keyCode] & 0x80) != 0;
}

bool InputClass::IsNumberPressed(int& outNumber)
{
	if (GetKey(DIK_0))
	{
		outNumber = 0;
		return true;
	}

	if (GetKey(DIK_1))
	{
		outNumber = 1;
		return true;
	}

	if (GetKey(DIK_2))
	{
		outNumber = 2;
		return true;
	}

	if (GetKey(DIK_3))
	{
		outNumber = 3;
		return true;
	}

	if (GetKey(DIK_4))
	{
		outNumber = 4;
		return true;
	}

	if (GetKey(DIK_5))
	{
		outNumber = 5;
		return true;
	}

	if (GetKey(DIK_6))
	{
		outNumber = 6;
		return true;
	}

	if (GetKey(DIK_7))
	{
		outNumber = 7;
		return true;
	}

	if (GetKey(DIK_8))
	{
		outNumber = 8;
		return true;
	}

	if (GetKey(DIK_9))
	{
		outNumber = 9;
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}

bool InputClass::IsMousePressed()
{
	// Check the left mouse button state.
	return m_mouseState.rgbButtons[0] & 0x80;
}