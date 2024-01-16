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

	bool Initialize(HWND);
	bool InitializeModel(HWND, ModelClass**, const char*);
	bool InitializeShader(HWND, ShaderClass**, const char*, const char*, bool clampSamplerMode = false);
	void InitializeGameObject(ModelClass*, ShaderClass*, TextureSetClass*, const char*, GameObjectClass** ptr = nullptr);
	void InitializeGameObject2D(BitmapClass*, ShaderClass*, GameObjectClass2D** ptr = nullptr);
	bool InitializeTextClass(TextClass** ptr, ShaderClass* shader, FontClass*, int maxLength);
	bool InitializeBitmap(BitmapClass**, const char*);
	void Shutdown();
	bool Frame(InputClass*);

private:
	bool Render();
	bool UpdateMouseStrings(int, int, bool);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	RenderClass* m_RenderClass;
	TimerClass* m_Timer;
	FpsClass* m_Fps;
	FrustumClass* m_Frustum;
	ShaderClass::ShaderParameters* m_Parameters;

	int m_numLights;
	std::chrono::high_resolution_clock::time_point m_startTime;

	FontClass* m_Font;
	LightClass* m_Lights, * m_DirLight;
	RenderTextureClass* m_RenderTexDisplay;
	DisplayPlaneClass* m_DisplayPlane;

	GameObjectClass* m_MadelineGO1, * m_MadelineGO2, * m_IcosphereGO, * m_mountainGO, * m_transIcoGO, * m_cubeGO, * m_fractalGO;
	GameObjectClass2D* m_spinnerGO2D, *m_cursorGO2D;
	BitmapClass* m_BitmapSpinner, *m_BitmapCursor;	
	TextClass* m_TextString1, * m_TextString2, * m_TextStringMouseX, * m_TextStringMouseY, * m_TextStringMouseBttn, * m_FpsString;
	TextureSetClass* m_TexSetMoss, * m_TexSetStars, * m_TexSetSnow, * m_TexSetReflection, * m_TexSetWater, * m_TexSetNone;

	vector<ModelClass*> m_ModelList;
	vector<ShaderClass*> m_ShaderList;
	vector<BitmapClass*> m_BitmapList;
};

#endif