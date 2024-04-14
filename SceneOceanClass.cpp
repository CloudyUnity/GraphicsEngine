#include "SceneOceanClass.h"
#include "ModelLineListClass.h"

SceneOceanClass::SceneOceanClass()
{
}

bool SceneOceanClass::InitializeScene(HWND hwnd)
{
	bool result;

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Initialize2DView();

	ID3D11Device* device = m_Direct3D->GetDevice();
	ID3D11DeviceContext* context = m_Direct3D->GetDeviceContext();

	bool clampSamplerMode = true;
	ShaderClass* shaderFractal = 0, * shaderLine = 0;
	ShaderTessClass* shaderOcean = 0, * shaderOceanTri = 0;
	result = 
		CreateShader(hwnd, &shaderOcean, "../GraphicsEngine/Ocean.vs", "../GraphicsEngine/Ocean.hs", "../GraphicsEngine/Ocean.ds", "../GraphicsEngine/Ocean.ps") &&
		CreateShader(hwnd, &shaderOceanTri, "../GraphicsEngine/Ocean.vs", "../GraphicsEngine/OceanTri.hs", "../GraphicsEngine/OceanTri.ds", "../GraphicsEngine/Ocean.ps") &&
		CreateShader(hwnd, &shaderLine, "../GraphicsEngine/Line.vs", "../GraphicsEngine/Line.ps") &&
		CreateShader(hwnd, &shaderFractal, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Fractal.ps");
	if (!result)
		return false;

	TextureSetClass* texSetOcean;
	CreateTexSet(&texSetOcean);
	texSetOcean->Add(device, context, "../GraphicsEngine/Data/Celeste.tga");

	vector<XMFLOAT3> debugLinePoints;
	debugLinePoints.push_back(XMFLOAT3(0, 1000, 0));
	debugLinePoints.push_back(XMFLOAT3(0, -1000, 0));

	ModelClass* modelPlane = 0, * modelCube = 0;
	ModelLineListClass* modelLine = 0;
	result = 
		CreateModel(hwnd, &modelPlane, "../GraphicsEngine/Models/Plane.txt") &&
		CreateModel(hwnd, &modelLine, debugLinePoints) &&
		CreateModel(hwnd, &modelCube, "../GraphicsEngine/Models/Cube.txt");
	if (!result)
		return false;

	bool usingTri = true;

	modelPlane->SetPrimitiveControlPointPatchList(usingTri ? 3 : 4);
	modelCube->SetPrimitiveControlPointPatchList(3);

	GameObjectClass* goTest;
	// CreateGameObject(modelCube, shaderOceanTri, texSetOcean, false, "Test", &goTest);

	GameObjectClass* debugLineGo;
	CreateGameObject(modelLine, shaderLine, nullptr, false, "Line", &debugLineGo);

	CreateGameObject(modelPlane, (usingTri ? shaderOceanTri : shaderOcean), texSetOcean, false, "Ocean", &m_GoOcean);
	m_GoOcean->SetScale(20, 1.5f, 20);
	m_GoOcean->SetPosition(0, -5, 10);
	m_GoOcean->SetBackCulling(false);

	m_DirLight = new LightClass();
	m_DirLight->SetAmbientColor(0.2f, 0.2f, 0.3f, 1.0f);
	m_DirLight->SetDiffuseColor(0.8f, 1.0f, 1.0f, 1.0f);
	m_DirLight->SetDirection(1, -0.5f, 0);
	m_DirLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLight->SetSpecularPower(32.0f);

	return true;
}

bool SceneOceanClass::Frame(InputClass*, float frameTime)
{
	return true;
}

bool SceneOceanClass::LateFrame(InputClass* input, float frameTime)
{
	m_Camera->Frame(input, frameTime, m_settings->m_CurrentData.CameraSpeed, m_settings->m_CurrentData.CameraRotationSpeed);

	return true;
}

void SceneOceanClass::SetParameters(ShaderClass::ShaderParameters* params)
{
	params->light.ambientColor = m_DirLight->GetAmbientColor();
	params->light.diffuseColor = m_DirLight->GetDiffuseColor();
	params->light.lightDirection = m_DirLight->GetDirection();
	params->light.specularColor = m_DirLight->GetSpecularColor();
	params->light.specularPower = m_DirLight->GetSpecularPower();

	params->camera.cameraPosition = m_Camera->GetPosition();
}

void SceneOceanClass::OnSwitchTo()
{
	m_RenderClass->SetCurrentCamera(m_Camera);
}

void SceneOceanClass::Shutdown()
{
	if (m_Camera)
	{
		delete m_Camera;
	}
}
