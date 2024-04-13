#include "SceneOceanClass.h"

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
	ShaderClass* shaderFractal = 0;
	ShaderTessClass* shaderOcean = 0, * shaderOceanTri = 0;
	result = 
		CreateShader(hwnd, &shaderOcean, "../GraphicsEngine/Ocean.vs", "../GraphicsEngine/Ocean.hs", "../GraphicsEngine/Ocean.ds", "../GraphicsEngine/Ocean.ps") &&
		CreateShader(hwnd, &shaderOceanTri, "../GraphicsEngine/Ocean.vs", "../GraphicsEngine/OceanTri.hs", "../GraphicsEngine/OceanTri.ds", "../GraphicsEngine/Ocean.ps") &&
		CreateShader(hwnd, &shaderFractal, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Fractal.ps");
	if (!result)
		return false;

	TextureSetClass* texSetOcean;
	CreateTexSet(&texSetOcean);
	texSetOcean->Add(device, context, "../GraphicsEngine/Data/Celeste.tga");

	ModelClass* modelPlane = 0, * modelCube = 0;
	result = 
		CreateModel(hwnd, &modelPlane, "../GraphicsEngine/Models/Plane.txt") &&
		CreateModel(hwnd, &modelCube, "../GraphicsEngine/Models/Cube.txt");
	if (!result)
		return false;

	modelPlane->SetPrimitiveControlPointPatchList(3);
	modelCube->SetPrimitiveControlPointPatchList(3);

	//GameObjectClass* goTest;
	//CreateGameObject(modelCube, shaderOceanTri, texSetOcean, false, "Test", &goTest);

	CreateGameObject(modelPlane, shaderOceanTri, texSetOcean, false, "Ocean", &m_GoOcean);
	m_GoOcean->SetScale(10, 1, 10);
	m_GoOcean->SetPosition(0, -5, 10);

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

void SceneOceanClass::SetParameters(ShaderClass::ShaderParameters*)
{
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
