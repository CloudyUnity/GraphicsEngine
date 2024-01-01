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
using std::vector;
using std::string;
using std::unordered_map;
using std::any;
using std::any_cast;

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

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
	ModelClass* m_MadelineModel, *m_IcosphereModel;
	GameObjectClass* m_MadelineObject1, * m_MadelineObject2, * m_IcosphereObject;
	GameObjectClass2D* m_SpinnerObj, *m_MouseObj;

	TextureShaderClass* m_TextureShader, * m_2DShader, * m_FontShader, * m_DisplayShader;
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
	RenderTextureClass* m_RenderTexture;
	DisplayPlaneClass* m_DisplayPlane;

	vector<GameObjectClass*> m_AllGameObjectList;
	vector<GameObjectClass2D*> m_All2DGameObjectList;
	vector<TextClass*> m_AllTextClassList;
};

#endif