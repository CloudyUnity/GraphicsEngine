#include "SceneOceanClass.h"

SceneOceanClass::SceneOceanClass()
{
}

bool SceneOceanClass::InitializeScene(HWND hwnd)
{
	bool result;

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 5.0f, -20.0f);
	m_Camera->Initialize2DView();

	float dirLightX = 0.0;
	float dirLightY = -0.5f;
	float dirLightZ = -1.0;

	m_DirLight = new LightClass();
	m_DirLight->SetAmbientColor(0.0f, 0.106f, 0.15294f, 1.0f);
	m_DirLight->SetDiffuseColor(0.2235f, 0.30588f, 0.34902f, 1.0f);
	m_DirLight->SetDirection(dirLightX, dirLightY, dirLightZ);
	m_DirLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLight->SetSpecularPower(32.0f);

	ID3D11Device* device = m_Direct3D->GetDevice();
	ID3D11DeviceContext* context = m_Direct3D->GetDeviceContext();

	// SHADERS

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

	// TEXSETS

	TextureSetClass* texSetOcean;
	CreateTexSet(&texSetOcean);
	texSetOcean->Add(device, context, "../GraphicsEngine/Data/Celeste.tga");

	// MODELS

	vector<XMFLOAT3> debugLinePoints;
	debugLinePoints.push_back(XMFLOAT3(dirLightX * 1000, dirLightY * 1000, dirLightZ * 1000));
	debugLinePoints.push_back(XMFLOAT3(dirLightX * -1000, dirLightY * -1000, dirLightZ * -1000));

	ModelClass* modelPlane = 0, * modelCube = 0;
	result = 
		CreateModel(hwnd, &modelPlane, "../GraphicsEngine/Models/Plane.txt") &&
		CreateModel(hwnd, &m_debugLineLight, debugLinePoints) &&
		CreateModel(hwnd, &m_debugLineNormal, debugLinePoints) &&
		CreateModel(hwnd, &m_debugLineTangent, debugLinePoints) &&
		CreateModel(hwnd, &m_debugLineBinormal, debugLinePoints) &&
		CreateModel(hwnd, &modelCube, "../GraphicsEngine/Models/Cube.txt");
	if (!result)
		return false;

	bool usingTri = true;

	modelPlane->SetPrimitiveControlPointPatchList(usingTri ? 3 : 4);
	modelPlane->SetCustomBoundingRadius(modelPlane->GetBoundingRadius() * 2.0f);
	modelCube->SetPrimitiveControlPointPatchList(3);

	// GOS

	GameObjectClass* goTest;
	// CreateGameObject(modelCube, shaderOceanTri, texSetOcean, false, "Test", &goTest);

	CreateGameObject(m_debugLineLight, shaderLine, nullptr, false, "LineYellow");
	CreateGameObject(m_debugLineNormal, shaderLine, nullptr, false, "LineBlue");
	//CreateGameObject(m_debugLineTangent, shaderLine, nullptr, false, "LineRed");
	//CreateGameObject(m_debugLineBinormal, shaderLine, nullptr, false, "LineGreen");

	bool detailMode = true;

	float size = detailMode ? 1.5f : 10;
	float gridSize = detailMode ? 20 : 40;
	XMFLOAT2 startPos = XMFLOAT2(-gridSize * size * 0.5f + size * 0.5f, -gridSize * size * 0.5f + size * 0.5f);
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			GameObjectClass* ptr;
			CreateGameObject(modelPlane, (usingTri ? shaderOceanTri : shaderOcean), texSetOcean, false, "Ocean" + i, ptr);
			ptr->SetScale(size, 1, size);
			ptr->SetPosition(startPos.x + i * size, 0, startPos.y + j * size);
			ptr->SetBackCulling(false);
		}
	}	

	return true;
}

bool SceneOceanClass::Frame(InputClass* input, float frameTime)
{
	float ddx = 0, ddz = 0;
	float posY = 0;

	for (int i = 0; i < SIN_COUNT; i++)
	{
		float a = m_oceanSines[i].x;
		float p = m_oceanSines[i].y;
		float freq = m_oceanSines[i].z;		
		float theta = m_oceanSines[i].w;
		XMFLOAT2 D = XMFLOAT2(cos(theta), sin(theta));

		float exponent = sin(p * m_time) - 1;
		posY += a * exp(exponent);

		float cosine = cos(p * m_time);
		ddx += a * freq * D.x * cosine * exp(exponent);
		ddz += a * freq * D.y * cosine * exp(exponent);
	}

	// XMFLOAT3 tangent = XMFLOAT3(1, 0, ddz);
	// XMFLOAT3 binormal = XMFLOAT3(0, 1, ddx);
	XMFLOAT3 normal = XMFLOAT3(-ddx, 1, -ddz);
	float mag = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	normal.x /= mag;
	normal.y /= mag;
	normal.z /= mag;

	vector<XMFLOAT3> points;

	//points.push_back(XMFLOAT3(tangent.x * 1000, tangent.y * 1000 + posY, tangent.z * 1000));
	//points.push_back(XMFLOAT3(0, posY, 0));
	//m_debugLineTangent->UpdateBuffers(m_Direct3D->GetDeviceContext(), points);
	//points.clear();

	//points.push_back(XMFLOAT3(binormal.x * 1000, binormal.y * 1000 + posY, binormal.z * 1000));
	//points.push_back(XMFLOAT3(0, posY, 0));
	//m_debugLineBinormal->UpdateBuffers(m_Direct3D->GetDeviceContext(), points);
	//points.clear();

	points.push_back(XMFLOAT3(normal.x * 1000, normal.y * 1000 + posY, normal.z * 1000));
	points.push_back(XMFLOAT3(0, posY, 0));
	m_debugLineNormal->UpdateBuffers(m_Direct3D->GetDeviceContext(), points);

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

	m_time = params->utils.time;
	// m_time = 28.447;

	for (int i = 0; i < SIN_COUNT; i++)
		m_oceanSines[i] = params->oceanSine.ampPhaseFreq[i];
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
