#ifndef _SCENETESTCLASS_H_
#define _SCENETESTCLASS_H_

#include "SceneClass.h"
#include "RenderClass.h"

class SceneTestClass : public SceneClass
{
public:
	SceneTestClass(string);

	bool InitializeScene(HWND hwnd) override;

	bool Frame(InputClass*, float frameTime) override;
	bool LateFrame(InputClass* input, float frameTime) override;
	void SetParameters(ShaderClass::ShaderParamsGlobalType*) override;
	void OnSwitchTo() override;
	void Shutdown();
	
	void SetDirLight(float x, float y, float z);
	void UpdatePortals(XMFLOAT3 camPos);

private:
	CameraClass* m_Camera;

	FontClass* m_Font;
	LightClass* m_Lights, * m_DirLight;
	DisplayPlaneClass* m_DisplayPlane, * m_DisplayPortal1, * m_DisplayPortal2, * m_DisplayPPBlur1, * m_DisplayPPBlur2;

	GameObjectClass* m_MadelineGO1, * m_IcosphereGO, * m_fractalGO, * m_skyboxGO, * m_testIcoGO;
	GameObjectClass2D* m_spinnerGO2D, * m_cursorGO2D;
	BitmapClass* m_BitmapSpinner, * m_BitmapCursor;
	TextClass* m_TextString1, * m_TextString2;
	ParticleSystemClass* m_PSRaindrops;

	XMVECTOR m_previousPortalOffset1;
	XMVECTOR m_previousPortalOffset2;
	XMMATRIX m_lightViewMatrix;
};

#endif