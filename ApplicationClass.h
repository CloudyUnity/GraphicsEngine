#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "shaderclass.h"
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
#include "Settings.h"
#include "ParticleSystemClass.h"
#include "IShutdown.h"
#include "SceneTestClass.h"
#include "SceneOceanClass.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::any;
using std::any_cast;
using std::unique_ptr;

class ApplicationClass : IShutdown
{
public:
	ApplicationClass();
	~ApplicationClass();

	bool Initialize(HWND);
	bool InitializeShader(HWND, ShaderClass**, const char*, const char*, bool clampSamplerMode = false);
	bool InitializeTextClass(TextClass** ptr, ShaderClass* shader, FontClass*, int maxLength);

	void UpdateParameters();

	void Shutdown() override;
	bool Frame(HWND hwnd, InputClass*);
	bool LateFrame(InputClass*, float);

	bool SwitchScene(HWND hwnd);

private:
	bool Render();
	bool UpdateMouseStrings(int, int, bool);	

private:
	D3DClass* m_Direct3D;	
	RenderClass* m_RenderClass;
	TimerClass* m_Timer;
	FpsClass* m_Fps;
	FrustumClass* m_Frustum;
	ShaderClass::ShaderParameters* m_Parameters;
	Settings* m_Settings;
	FontClass* m_Font; // Shutdown

	std::chrono::high_resolution_clock::time_point m_startTime;	
	TextClass* m_FpsString, * m_TextStringMouseX, * m_TextStringMouseY, * m_TextStringMouseBttn;

	vector<IShutdown*> m_loadedAssetsList;
	vector<TextClass*> m_overlayTextList;

	SceneClass* m_currentScene;
	vector<SceneClass*> m_sceneList;
};

#endif