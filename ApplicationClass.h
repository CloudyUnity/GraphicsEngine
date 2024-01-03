#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"
#include "textureshaderclass.h"
#include "lightclass.h"
#include "bitmapclass.h"
#include "timerclass.h"
#include "fontclass.h"
#include "textclass.h"
#include "fpsclass.h"
#include "InputClass.h"
#include <vector>
#include <chrono>
#include "GameObjectClass.h"
#include "GameObjectClass2D.h"
#include "FrustumClass.h"
#include "rendertextureclass.h"
#include "displayplaneclass.h"
#include "RenderClass.h"
using std::vector;
using std::string;
using std::unordered_map;
using std::any;
using std::any_cast;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass*);

private:
	bool Render();
	bool UpdateFps();
	bool UpdateMouseStrings(int, int, bool);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_ModelMadeline, * m_ModelIcosphere, * m_ModelMountain, * m_ModelCube;
	GameObjectClass* m_MadelineGO1, * m_MadelineGO2, * m_IcosphereGO, * m_mountainGO, * m_transIcoGO, * m_cubeGO;
	GameObjectClass2D* m_SpinnerObj, *m_MouseObj;

	RenderClass* m_RenderClass;

	ShaderClass* m_ShaderMain, * m_Shader2D, * m_ShaderFont, *m_ShaderReflect;
	LightClass* m_Lights, *m_DirLight;
	int m_numLights;
	BitmapClass* m_Bitmap, *m_MouseCursor;
	TimerClass* m_Timer;
	FontClass* m_Font;
	TextClass* m_TextString1, * m_TextString2, * m_TextStringMouseX, * m_TextStringMouseY, * m_TextStringMouseBttn;
	FpsClass* m_Fps;
	TextClass* m_FpsString;
	int m_previousFps;
	std::chrono::high_resolution_clock::time_point m_startTime;
	FrustumClass* m_Frustum;
	RenderTextureClass* m_RenderTexDisplay, * m_RenderTexReflection;
	DisplayPlaneClass* m_DisplayPlane;
	TextureSetClass* m_TexSetMoss, * m_TexSetStars, * m_TexSetSnow, * m_TexSetReflection;
};

#endif