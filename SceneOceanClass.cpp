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
	ShaderClass* shaderFractal = 0, * shaderLine = 0, * shaderSkybox = 0, * shaderFog = 0;
	ShaderTessClass* shaderOcean = 0, * shaderOceanTri = 0;
	result = 
		CreateShader(hwnd, &shaderOcean, "../GraphicsEngine/Ocean.vs", "../GraphicsEngine/Ocean.hs", "../GraphicsEngine/Ocean.ds", "../GraphicsEngine/Ocean.ps") &&
		CreateShader(hwnd, &shaderOceanTri, "../GraphicsEngine/Ocean.vs", "../GraphicsEngine/OceanTri.hs", "../GraphicsEngine/OceanTri.ds", "../GraphicsEngine/Ocean.ps") &&
		CreateShader(hwnd, &shaderLine, "../GraphicsEngine/Line.vs", "../GraphicsEngine/Line.ps") &&
		CreateShader(hwnd, &shaderSkybox, "../GraphicsEngine/Skybox.vs", "../GraphicsEngine/Skybox.ps", clampSamplerMode) &&
		CreateShader(hwnd, &shaderFog, "../GraphicsEngine/Fog.vs", "../GraphicsEngine/Fog.ps") &&
		CreateShader(hwnd, &shaderFractal, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Fractal.ps");
	if (!result)
		return false;

	// TEXSETS

	TextureSetClass* texSetSnow, * texSetSkybox;
	CreateTexSet(&texSetSnow);
	texSetSnow->Add(device, context, "../GraphicsEngine/Data/White.tga");
	texSetSnow->Add(device, context, "../GraphicsEngine/Data/White.tga");
	texSetSnow->Add(device, context, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetSnow->Add(device, context, "../GraphicsEngine/Data/DefaultNormal.tga");

	CreateTexSet(&texSetSkybox);
	result = texSetSkybox->AddCubemap(device, context, "../GraphicsEngine/Data/Skybox/Skybox.tga");
	if (!result)
		return false;

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

	// GOS

	GameObjectClass* goWall1, * goWall2, * goWall3, * goWall4, *goLineYellow, *goLineBlue;

	CreateGameObject(m_debugLineLight, shaderLine, nullptr, false, "LineYellow", goLineYellow);
	goLineYellow->m_shaderUniformData.pixel.pixelColor = XMFLOAT4(1, 1, 0, 1);
	CreateGameObject(m_debugLineNormal, shaderLine, nullptr, false, "LineBlue", goLineBlue);
	goLineBlue->m_shaderUniformData.pixel.pixelColor = XMFLOAT4(0, 0, 1, 1);

	bool detailMode = true;

	float size = detailMode ? 2.0f : 10;
	float halfSize = size * 0.5f;
	float gridSize = detailMode ? 10 : 20;
	XMFLOAT2 startPos = XMFLOAT2(-gridSize * halfSize + halfSize, -gridSize * halfSize + halfSize);
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			GameObjectClass* ptr;
			CreateGameObject(modelPlane, (usingTri ? shaderOceanTri : shaderOcean), texSetSkybox, false, "Ocean" + i, ptr);
			ptr->SetScale(size, 1, size);
			ptr->SetPosition(startPos.x + i * size, 0, startPos.y + j * size);
			ptr->m_shaderUniformData.tesselation.tessellationAmount = 64;
		}
	}	

	CreateGameObject(modelCube, shaderSkybox, texSetSkybox, false, "Skybox", m_skyboxGO);
	m_skyboxGO->SetScale(500);
	m_skyboxGO->SetBackCulling(false);

	float wallSize = 1;
	float wallHeight = 0.5f;
	float halfWallSize = wallSize * 0.5f;
	float waterSize = gridSize * size;
	float scalingFactorLong = 0.5f * (waterSize + wallSize * 6) / wallSize;
	float scalingFactorShort = 0.5f * (waterSize + wallSize * 2) / wallSize;
	CreateGameObject(modelCube, shaderFog, texSetSnow, false, "Wall1", goWall1);
	goWall1->SetPosition(startPos.x - size - wallSize, wallHeight, startPos.y - halfSize + waterSize * 0.5f);
	goWall1->SetScale(wallSize, wallSize, scalingFactorLong);

	CreateGameObject(modelCube, shaderFog, texSetSnow, false, "Wall2", goWall2);
	goWall2->SetPosition(startPos.x + waterSize + wallSize, wallHeight, startPos.y - halfSize + waterSize * 0.5f);
	goWall2->SetScale(wallSize, wallSize, scalingFactorLong);

	CreateGameObject(modelCube, shaderFog, texSetSnow, false, "Wall3", goWall3);
	goWall3->SetPosition(startPos.x - halfSize + waterSize * 0.5f, wallHeight, startPos.y - size - wallSize);
	goWall3->SetScale(scalingFactorShort, wallSize, wallSize);

	CreateGameObject(modelCube, shaderFog, texSetSnow, false, "Wall4", goWall4);
	goWall4->SetPosition(startPos.x - halfSize + waterSize * 0.5f, wallHeight, startPos.y + waterSize + wallSize);
	goWall4->SetScale(scalingFactorShort, wallSize, wallSize);

	return true;
}

bool SceneOceanClass::Frame(InputClass* input, float frameTime)
{
	XMFLOAT3 camPos = m_Camera->GetPosition();

	if (m_skyboxGO && !m_settings->m_CurrentData.FreezeSkybox)
		m_skyboxGO->SetPosition(camPos.x, camPos.y, camPos.z);
	if (m_skyboxGO)
		m_skyboxGO->SetScale(m_settings->m_CurrentData.ShadowsEnabled ? 500 : 0);

	float ddx = 0, ddz = 0;
	float lastDDX = 0, lastDDZ = 0;
	float posY = 0;

	for (int i = 0; i < SIN_COUNT; i++)
	{
		float a = m_oceanSines[i].x;
		float p = m_oceanSines[i].y;
		float freq = m_oceanSines[i].z;		
		float theta = m_oceanSines[i].w;
		XMFLOAT2 D = XMFLOAT2(cos(theta), sin(theta));
		float align = D.x * lastDDX + D.y * lastDDZ;

		float exponent = sin(p * m_time + align * freq) - 1;
		posY += a * exp(exponent);

		float cosine = cos(p * m_time + align * freq);
		lastDDX = a * freq * D.x * cosine * exp(exponent);
		lastDDZ = a * freq * D.y * cosine * exp(exponent);
		ddx += lastDDX;
		ddz += lastDDZ;
	}

	XMFLOAT3 normal = XMFLOAT3(-ddx, 1, -ddz);
	float mag = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	normal.x /= mag;
	normal.y /= mag;
	normal.z /= mag;

	vector<XMFLOAT3> points;

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

void SceneOceanClass::SetParameters(ShaderClass::ShaderParamsGlobalType* params)
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
