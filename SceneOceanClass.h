#ifndef _SCENEOCEAN_H_
#define _SCENEOCEAN_H_

#include "SceneClass.h"
#include "RenderClass.h"
#include "ModelLineListClass.h"

class SceneOceanClass : public SceneClass
{
public:
	SceneOceanClass();

	bool InitializeScene(HWND hwnd) override;

	bool Frame(InputClass*, float frameTime) override;
	bool LateFrame(InputClass* input, float frameTime) override;
	void SetParameters(ShaderClass::ShaderParamsGlobalType*) override;
	void OnSwitchTo() override;
	void Shutdown();

private:
	CameraClass* m_Camera;

	LightClass* m_DirLight;

	GameObjectClass* m_GoOcean, * m_skyboxGO;
	ModelLineListClass* m_debugLineLight, *m_debugLineOrigin, *m_debugLineBinormal, *m_debugLineNormal;
	XMFLOAT4 m_oceanSines[SIN_COUNT];
	float m_time;
};

#endif