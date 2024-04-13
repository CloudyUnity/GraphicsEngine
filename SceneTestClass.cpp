#include "SceneTestClass.h"

SceneTestClass::SceneTestClass()
{
	m_Camera = 0;
	m_Font = 0;

	m_Lights = 0;
	m_DirLight = 0;

	m_BitmapSpinner = 0;
	m_BitmapCursor = 0;

	m_TextString1 = 0;
	m_TextString2 = 0;

	m_DisplayPlane = 0;

	m_mountainGO = 0;
	m_IcosphereGO = 0;
	m_transIcoGO = 0;
	m_MadelineGO1 = 0;
	m_MadelineGO2 = 0;
	m_cubeGO = 0;
	m_fractalGO = 0;
	m_skyboxGO = 0;

	m_cursorGO2D = 0;
	m_spinnerGO2D = 0;
}

bool SceneTestClass::InitializeScene(HWND hwnd)
{
	bool result;

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Initialize2DView();

	ID3D11Device* device = m_Direct3D->GetDevice();
	ID3D11DeviceContext* context = m_Direct3D->GetDeviceContext();

	// SHADERS	

	bool clampSamplerMode = true;
	ShaderClass* shaderMain = 0, * shaderReflect = 0, * shaderWater = 0, * shader2D = 0, * shaderFont = 0,
		* shaderFractal = 0, * shaderFire = 0, * shaderBlur = 0, * shaderFilter = 0, * shaderSkybox = 0, * shaderPS = 0,
		* shaderPortal = 0;
	result =
		CreateShader(hwnd, &shaderMain, "../GraphicsEngine/Fog.vs", "../GraphicsEngine/Fog.ps") &&
		CreateShader(hwnd, &shaderReflect, "../GraphicsEngine/Reflect.vs", "../GraphicsEngine/Reflect.ps") &&
		CreateShader(hwnd, &shaderWater, "../GraphicsEngine/Water.vs", "../GraphicsEngine/Water.ps") &&
		CreateShader(hwnd, &shader2D, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/2D.ps") &&
		CreateShader(hwnd, &shaderFont, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Font.ps") &&
		CreateShader(hwnd, &shaderFire, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Fire.ps", clampSamplerMode) &&
		CreateShader(hwnd, &shaderBlur, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Blur.ps") &&
		CreateShader(hwnd, &shaderFilter, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Filter.ps") &&
		CreateShader(hwnd, &shaderSkybox, "../GraphicsEngine/Skybox.vs", "../GraphicsEngine/Skybox.ps", clampSamplerMode) &&
		CreateShader(hwnd, &shaderPS, "../GraphicsEngine/Particle.vs", "../GraphicsEngine/Particle.ps") &&
		CreateShader(hwnd, &shaderPortal, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Portal.ps") &&
		CreateShader(hwnd, &shaderFractal, "../GraphicsEngine/Fog.vs", "../GraphicsEngine/Fractal.ps");
	if (!result)
		return false;

	// TEXSETS

	TextureSetClass* texSetMoss, * texSetStars, * texSetSnow, * texSetReflection, * texSetWater, * texSetNone, * texSetFire, * texSetSkybox, * texSetPS;
	CreateTexSet(&texSetMoss);
	CreateTexSet(&texSetStars);
	CreateTexSet(&texSetSnow);
	CreateTexSet(&texSetReflection);
	CreateTexSet(&texSetWater);
	CreateTexSet(&texSetNone);
	CreateTexSet(&texSetFire);
	CreateTexSet(&texSetSkybox);
	CreateTexSet(&texSetPS);

	texSetMoss->Add(device, context, "../GraphicsEngine/Data/Celeste.tga");
	texSetMoss->Add(device, context, "../GraphicsEngine/Data/Moss.tga");
	texSetMoss->Add(device, context, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetMoss->Add(device, context, "../GraphicsEngine/Data/MossNormal.tga");

	texSetStars->Add(device, context, "../GraphicsEngine/Data/Celeste.tga");
	texSetStars->Add(device, context, "../GraphicsEngine/Data/Celeste.tga");
	texSetStars->Add(device, context, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetStars->Add(device, context, "../GraphicsEngine/Data/RockNormal.tga");

	texSetSnow->Add(device, context, "../GraphicsEngine/Data/Snow.tga");
	texSetSnow->Add(device, context, "../GraphicsEngine/Data/Snow.tga");
	texSetSnow->Add(device, context, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetSnow->Add(device, context, "../GraphicsEngine/Data/DefaultNormal.tga");

	texSetReflection->Add(device, context, "../GraphicsEngine/Data/Glass.tga");
	texSetReflection->Add(device, context, "../GraphicsEngine/Data/Glass.tga");
	texSetReflection->Add(device, context, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetReflection->Add(device, context, "../GraphicsEngine/Data/GlassNormal.tga");

	texSetWater->Add(device, context, "../GraphicsEngine/Data/WaterBlue.tga");
	texSetWater->Add(device, context, "../GraphicsEngine/Data/WaterBlue.tga");
	texSetWater->Add(device, context, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetWater->Add(device, context, "../GraphicsEngine/Data/WaterNormal.tga");

	texSetFire->Add(device, context, "../GraphicsEngine/Data/NoiseAlt.tga");
	texSetFire->Add(device, context, "../GraphicsEngine/Data/fireColor.tga");
	texSetFire->Add(device, context, "../GraphicsEngine/Data/flameAlpha.tga");

	result = texSetSkybox->AddCubemap(device, context, "../GraphicsEngine/Data/Skybox/Skybox.tga");
	if (!result)
		return false;

	texSetPS->Add(device, context, "../GraphicsEngine/Data/Sparkle.tga");

	// MODELS

	ModelClass* modelMadeline = 0, * modelMountain = 0, * modelCube = 0, * modelIcosphere = 0, * modelPlane = 0;
	result =
		CreateModel(hwnd, &modelMadeline, "../GraphicsEngine/Models/Madeline.txt") &&
		CreateModel(hwnd, &modelMountain, "../GraphicsEngine/Models/MountFuji.txt") &&
		CreateModel(hwnd, &modelCube, "../GraphicsEngine/Models/Cube.txt") &&
		CreateModel(hwnd, &modelIcosphere, "../GraphicsEngine/Models/Icosphere.txt") &&
		CreateModel(hwnd, &modelPlane, "../GraphicsEngine/Models/Plane.txt");
	if (!result)
		return false;

	// BITMAPS

	result =
		CreateBitmap(&m_BitmapSpinner, "../GraphicsEngine/Animations/Spinner.txt") &&
		CreateBitmap(&m_BitmapCursor, "../GraphicsEngine/Animations/MouseAnim.txt");
	if (!result)
		return false;

	// GAMEOBJECTS

	float waterSize = 2;
	bool opaque = false, transparent = true;

	GameObjectClass* waterGO = 0, * waterCubeGO = 0, * fireGO, * floorGO;	
	CreateGameObject(modelMadeline, shaderMain, texSetMoss, opaque, "Madeline1", &m_MadelineGO1);
	CreateGameObject(modelMadeline, shaderMain, texSetStars, opaque, "Madeline2", &m_MadelineGO2);
	CreateGameObject(modelIcosphere, shaderMain, texSetMoss, opaque, "IcosphereBig", &m_IcosphereGO);
	CreateGameObject(modelIcosphere, shaderMain, texSetSnow, transparent, "IcosphereTrans", &m_transIcoGO);
	CreateGameObject(modelMountain, shaderMain, texSetSnow, opaque, "Mountain", &m_mountainGO);
	CreateGameObject(modelPlane, shaderWater, texSetWater, transparent, "Water", &waterGO);
	CreateGameObject(modelCube, shaderReflect, texSetReflection, opaque, "GlassCube", &m_cubeGO);
	CreateGameObject(modelCube, shaderMain, texSetMoss, opaque, "WaterCube", &waterCubeGO);
	CreateGameObject(modelCube, shaderFractal, texSetNone, opaque, "Fractal", &m_fractalGO);
	CreateGameObject(modelPlane, shaderFire, texSetFire, transparent, "Fire", &fireGO);
	CreateGameObject(modelPlane, shaderMain, texSetSnow, opaque, "Floor", &floorGO);
	CreateGameObject(modelCube, shaderSkybox, texSetSkybox, opaque, "Skybox", &m_skyboxGO);
	CreateGameObject(modelIcosphere, shaderMain, texSetSnow, opaque, "Test", &m_testIcoGO);

	m_MadelineGO2->SetPosition(3, 0, 3);
	m_MadelineGO2->SetScale(0.5f, 0.5f, 0.5f);

	m_IcosphereGO->SetScale(5, 5, 5);
	m_IcosphereGO->SetPosition(0, 0, 15);
	m_IcosphereGO->SetBackCulling(false);

	m_transIcoGO->SetPosition(3, 0.5f, 3);
	m_transIcoGO->SetScale(1.0f, 1.0f, 1.0f);

	m_mountainGO->SetScale(0.3f, 0.3f, 0.3f);
	m_mountainGO->SetPosition(1, -13, 15);

	m_cubeGO->SetScale(1.5f, 0.01f, 1.5f);
	m_cubeGO->SetPosition(0, -0.5f, 0);

	waterGO->SetPosition(6, 0.5f, -0.5f);
	waterGO->SetScale(waterSize, 0.000000000001f, waterSize);

	waterCubeGO->SetScale(0.5f, 0.5f, 0.5f);
	waterCubeGO->SetPosition(6, 0.5f, -0.5f);

	m_fractalGO->SetPosition(25, 0, 0);
	m_fractalGO->SetScale(8);

	fireGO->SetPosition(-5, 0, 0);
	fireGO->SetRotation(-90, 0, 0);
	fireGO->SetBillBoarding(true);

	floorGO->SetPosition(0, -10, 0);
	floorGO->SetScale(25, 1, 25);

	m_skyboxGO->SetScale(500);
	m_skyboxGO->SetBackCulling(false);

	m_testIcoGO->SetScale(0.3f);
	m_testIcoGO->SetPosition(999, 999, 999);

	// SUBSCRIPTIONS

	int texSetIndex = 4;
	int format = 1;
	SubscribeToReflection(m_cubeGO, texSetIndex, format);
	SubscribeToReflection(waterGO, texSetIndex, format);

	texSetIndex = 5;
	SubscribeToRefraction(waterGO, texSetIndex, format);

	texSetIndex = 4;
	SubscribeToShadow(m_MadelineGO1, texSetIndex);
	SubscribeToShadow(floorGO, texSetIndex);
	SubscribeToShadow(m_IcosphereGO, texSetIndex);
	SubscribeToShadow(m_transIcoGO, texSetIndex);
	SubscribeToShadow(m_mountainGO, texSetIndex);
	SubscribeToShadow(waterCubeGO, texSetIndex);

	// 2D GAMEOBJECTS

	CreateGameObject2D(m_BitmapSpinner, shader2D, &m_spinnerGO2D);
	CreateGameObject2D(m_BitmapCursor, shader2D, &m_cursorGO2D);

	m_spinnerGO2D->SetPosition(1000, 50);
	m_spinnerGO2D->SetScale(0.75f, 0.75f);

	m_cursorGO2D->SetScale(0.1f, 0.1f);
	m_cursorGO2D->SetRotation(225);

	// LIGHTS

	m_Lights = new LightClass[NUM_POINT_LIGHTS];

	m_Lights[0].SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_Lights[0].SetPosition(100.0f, 0.0f, 0.0f);

	m_Lights[1].SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_Lights[1].SetPosition(100.0f, 0.0f, 0.0f);

	m_Lights[2].SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_Lights[2].SetPosition(100.0f, 0.0f, 0.0f);

	m_Lights[3].SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_Lights[3].SetPosition(100.0f, 0.0f, 0.0f);

	m_DirLight = new LightClass();
	m_DirLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_DirLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLight->SetDirection(0, 0, 0);
	m_DirLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLight->SetSpecularPower(32.0f);

	// FONTS

	m_Font = new FontClass;
	result = m_Font->Initialize(device, context, 0);
	if (!result)
		return false;

	// TEXT

	int maxLength = 32;
	result =
		CreateText(&m_TextString1, shaderFont, m_Font, maxLength) &&
		CreateText(&m_TextString2, shaderFont, m_Font, maxLength);
	if (!result)
		return false;

	m_TextString1->SetColor(1, 0, 0);
	m_TextString1->SetPosition(10, 10);
	m_TextString1->SetText("Example Text");
	m_TextString1->UpdateText();

	m_TextString2->SetColor(0, 1, 0);
	m_TextString2->SetPosition(15, 30);
	m_TextString2->SetText("More Text");
	m_TextString2->UpdateText();

	// RENDER TEXTURES

	int baseTexScale = 256;
	int downScaledWidth = (int)(SCREEN_X * m_settings->m_CurrentData.DownScaleMult);
	int downScaledHeight = (int)(SCREEN_Y * m_settings->m_CurrentData.DownScaleMult);
	format = 1;

	int portalResolution = 1024;

	RenderTextureClass* rendMainDisplay=0, *rendPortal1=0, *rendPortal2=0, *rendDepth = 0, *rendPP1 = 0, *rendPP2 = 0, *rendPP3 = 0;
	result = 
		CreateRenderTexture(&rendMainDisplay, device, baseTexScale, baseTexScale, SCREEN_DEPTH, SCREEN_NEAR, format) &&
		CreateRenderTexture(&rendPortal1, device, portalResolution, portalResolution, SCREEN_DEPTH, SCREEN_NEAR, format) &&
		CreateRenderTexture(&rendPortal2, device, portalResolution, portalResolution, SCREEN_DEPTH, SCREEN_NEAR, format) &&
		CreateRenderTexture(&rendPP1, device, downScaledWidth, downScaledHeight, SCREEN_DEPTH, SCREEN_NEAR, format) &&
		CreateRenderTexture(&rendPP2, device, downScaledWidth, downScaledHeight, SCREEN_DEPTH, SCREEN_NEAR, format) &&
		CreateRenderTexture(&rendPP3, device, downScaledWidth, downScaledHeight, SCREEN_DEPTH, SCREEN_NEAR, format);
	if (!result)
		return false;

	// DISPLAY PLANES

	float displayWidth = 1.0f;
	float displayHeight = 1.0f;
	bool postProcess = true;

	DisplayPlaneClass* displayPP1 = 0, * displayPP2 = 0, * displayPP3 = 0;	
	result = 
		CreateDisplayPlane(&m_DisplayPlane, device, displayWidth, displayHeight, rendMainDisplay, shader2D, "MainDisplay", m_Camera) &&
		CreateDisplayPlane(&displayPP1, device, SCREEN_X, SCREEN_Y, rendPP1, shaderBlur, "Display PP1", postProcess) &&
		CreateDisplayPlane(&displayPP2, device, SCREEN_X, SCREEN_Y, rendPP2, shaderBlur, "Display PP2", postProcess) &&
		CreateDisplayPlane(&displayPP3, device, SCREEN_X, SCREEN_Y, rendPP3, shaderFilter, "Display PP3", postProcess) &&
		CreateDisplayPlane(&m_DisplayPortal1, device, displayWidth, displayHeight, rendPortal1, shaderPortal, "Display Portal1") &&
		CreateDisplayPlane(&m_DisplayPortal2, device, displayWidth, displayHeight, rendPortal2, shaderPortal, "Display Portal2");
	if (!result)
		return false;
	m_DisplayPlane->SetPosition(0, 0, 5);

	float displaySize = 0.5f;
	displayPP1->SetScale(displaySize, displaySize, displaySize);
	displayPP2->SetScale(displaySize, displaySize, displaySize);
	displayPP3->SetScale(displaySize, displaySize, displaySize);

	m_DisplayPortal1->SetPosition(8, 0.0f, 0);
	m_DisplayPortal1->SetRotation(0, 0, 0);
	m_DisplayPortal1->SetScale(2);

	m_DisplayPortal2->SetPosition(-8, 0, 0);
	m_DisplayPortal2->SetRotation(0, 0, 0);
	m_DisplayPortal2->SetScale(2);

	// PARTICLE SYSTEMS

	ParticleSystemClass::ParticleSystemData data = {};
	data.maxParticles = 2000;
	data.particlesPerSecond = 100;
	data.size = 0.3f;
	data.sizeDev = 0.1f;
	data.posDevX = 40;
	data.posDevZ = 40;
	data.speed = 20;
	data.speedDev = 5.0f;
	data.velocityY = -1;
	data.velocityX = 0.1f;
	data.lifetime = 4.0f;
	data.lifetimeDev = 0.1f;
	data.sizeOverLifetimeCurveIndex = 5;
	data.colB = 1;
	data.colR = 1;
	data.colG = 1;

	result = CreateParticleSystem(&m_PSRaindrops, data, shaderPS, texSetPS, "PS1");
	if (!result)
		return false;

	return true;
}

bool SceneTestClass::Frame(InputClass* input, float frameTime)
{
	m_BitmapSpinner->Update(frameTime);

	XMFLOAT3 camPos = m_Camera->GetPosition();
	XMFLOAT3 camRot = m_Camera->GetRotation();

	static float time = 0.0f;
	time += 0.1f;

	static float rotation = 0.0f;
	rotation -= 0.0174532925f;
	if (rotation < 0.0f)
		rotation += 360.0f;

	float sunSpeed = 0.005f;
	SetDirLight(cos(time * sunSpeed), -0.75f, sin(time * sunSpeed));

	if (m_MadelineGO1)
		m_MadelineGO1->SetRotation(0, rotation * 10.0f, 0);
	if (m_IcosphereGO)
		m_IcosphereGO->SetRotation(rotation * 50.0f, 0, 0);
	if (m_fractalGO)
		m_fractalGO->SetRotation(0, rotation * sin(time * 0.01f) * 5, 0);

	if (m_skyboxGO && !m_settings->m_CurrentData.FreezeSkybox)
		m_skyboxGO->SetPosition(camPos.x, camPos.y, camPos.z);
	if (m_skyboxGO)
		m_skyboxGO->SetScale(m_settings->m_CurrentData.ShadowsEnabled ? 500 : 0);

	int mouseX, mouseY;
	input->GetMouseLocation(mouseX, mouseY);

	m_cursorGO2D->SetPosition(mouseX - 635.0f, -mouseY + 400.0f);

	if (m_PSRaindrops)
	{
		m_PSRaindrops->m_data.posX = camPos.x;
		m_PSRaindrops->m_data.posY = camPos.y + 20;
		m_PSRaindrops->m_data.posZ = camPos.z;
	}

	UpdatePortals(camPos);

	return true;
}

bool SceneTestClass::LateFrame(InputClass* input, float frameTime)
{
	m_Camera->Frame(input, frameTime, m_settings->m_CurrentData.CameraSpeed, m_settings->m_CurrentData.CameraRotationSpeed);

	return true;
}

void SceneTestClass::SetDirLight(float x, float y, float z)
{
	m_DirLight->SetDirection(x, y, z);

	XMVECTOR lightPos = XMVectorSet(x, y, z, 0);
	lightPos = XMVectorScale(lightPos, -m_settings->m_CurrentData.ShadowMapDistance);
	m_lightViewMatrix = XMMatrixLookAtLH(lightPos, XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));	
}

void SceneTestClass::SetParameters(ShaderClass::ShaderParameters* params)
{
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		params->lightColor.diffuseColor[i] = m_Lights[i].GetDiffuseColor();
		params->lightPos.lightPosition[i] = m_Lights[i].GetPosition();
	}

	params->light.ambientColor = m_DirLight->GetAmbientColor();
	params->light.diffuseColor = m_DirLight->GetDiffuseColor();
	params->light.lightDirection = m_DirLight->GetDirection();
	params->light.specularColor = m_DirLight->GetSpecularColor();
	params->light.specularPower = m_DirLight->GetSpecularPower();

	params->camera.cameraPosition = m_Camera->GetPosition();

	params->shadow.shadowView = m_lightViewMatrix;
}

void SceneTestClass::OnSwitchTo()
{
	m_RenderClass->SetCurrentCamera(m_Camera);
}

void SceneTestClass::Shutdown()
{
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	if (m_Lights)
	{
		delete[] m_Lights;
		m_Lights = 0;
	}

	if (m_DirLight)
	{
		delete m_DirLight;
		m_DirLight = 0;
	}
}

void SceneTestClass::UpdatePortals(XMFLOAT3 camPos)
{
	XMVECTOR _, translation, rotQuat;
	XMFLOAT3 rot3, trans3;

	XMMATRIX p1World = m_DisplayPortal1->GetWorldMatrix();
	XMMATRIX p2World = m_DisplayPortal2->GetWorldMatrix();

	// =

	XMMATRIX p2CamMatrix = m_Camera->GetWorldMatrix();
	p2CamMatrix *= XMMatrixInverse(nullptr, p2World);
	p2CamMatrix *= p1World;

	XMMatrixDecompose(&_, &rotQuat, &translation, p2CamMatrix);
	XMStoreFloat3(&rot3, rotQuat);
	XMStoreFloat3(&trans3, translation);

	m_DisplayPortal2->SetCameraPosAndRot(trans3.x, trans3.y, trans3.z,
		rot3.x * RAD_TO_DEG, rot3.y * RAD_TO_DEG, rot3.z * RAD_TO_DEG);

	// =

	XMMATRIX p1CamMatrix = m_Camera->GetWorldMatrix();
	p1CamMatrix *= XMMatrixInverse(nullptr, p1World);
	p1CamMatrix *= p2World;

	XMMatrixDecompose(&_, &rotQuat, &translation, p1CamMatrix);
	XMStoreFloat3(&rot3, rotQuat);
	XMStoreFloat3(&trans3, translation);

	m_DisplayPortal1->SetCameraPosAndRot(trans3.x, trans3.y, trans3.z,
		rot3.x * RAD_TO_DEG, rot3.y * RAD_TO_DEG, rot3.z * RAD_TO_DEG);

	// =

	int portalSide;
	int portalSideOld;

	XMFLOAT3 offsetFromPortal1 = XMFLOAT3(camPos.x - m_DisplayPortal1->m_PosX, camPos.y - m_DisplayPortal1->m_PosY, camPos.z - m_DisplayPortal1->m_PosZ);
	XMVECTOR offsetVector1 = XMLoadFloat3(&offsetFromPortal1);

	XMFLOAT3 offsetFromPortal2 = XMFLOAT3(camPos.x - m_DisplayPortal2->m_PosX, camPos.y - m_DisplayPortal2->m_PosY, camPos.z - m_DisplayPortal2->m_PosZ);
	XMVECTOR offsetVector2 = XMLoadFloat3(&offsetFromPortal2);

	float d = sqrt(offsetFromPortal1.x * offsetFromPortal1.x + offsetFromPortal1.y * offsetFromPortal1.y + offsetFromPortal1.z * offsetFromPortal1.z);
	portalSide = XMVectorGetX(XMVector3Dot(offsetVector1, m_DisplayPortal1->GetForwardVector())) >= 0 ? 1 : -1;
	portalSideOld = XMVectorGetX(XMVector3Dot(m_previousPortalOffset1, m_DisplayPortal1->GetForwardVector())) >= 0 ? 1 : -1;
	m_previousPortalOffset1 = offsetVector1;

	if (portalSide != portalSideOld && d < 2)
	{
		m_Camera->SetPosition(
			m_DisplayPortal2->m_PosX + offsetFromPortal1.x,
			m_DisplayPortal2->m_PosY + offsetFromPortal1.y,
			m_DisplayPortal2->m_PosZ + offsetFromPortal1.z);

		camPos = m_Camera->GetPosition();
		XMFLOAT3 offsetFromPortal1 = XMFLOAT3(camPos.x - m_DisplayPortal1->m_PosX, camPos.y - m_DisplayPortal1->m_PosY, camPos.z - m_DisplayPortal1->m_PosZ);
		XMVECTOR offsetVector1 = XMLoadFloat3(&offsetFromPortal1);
		XMFLOAT3 offsetFromPortal2 = XMFLOAT3(camPos.x - m_DisplayPortal2->m_PosX, camPos.y - m_DisplayPortal2->m_PosY, camPos.z - m_DisplayPortal2->m_PosZ);
		XMVECTOR offsetVector2 = XMLoadFloat3(&offsetFromPortal2);
		m_previousPortalOffset1 = offsetVector1;
		m_previousPortalOffset2 = offsetVector2;

		return;
	}

	d = sqrt(offsetFromPortal2.x * offsetFromPortal2.x + offsetFromPortal2.y * offsetFromPortal2.y + offsetFromPortal2.z * offsetFromPortal2.z);
	portalSide = XMVectorGetX(XMVector3Dot(offsetVector2, m_DisplayPortal2->GetForwardVector())) >= 0 ? 1 : -1;
	portalSideOld = XMVectorGetX(XMVector3Dot(m_previousPortalOffset2, m_DisplayPortal2->GetForwardVector())) >= 0 ? 1 : -1;
	m_previousPortalOffset2 = offsetVector2;

	if (portalSide != portalSideOld && d < 2)
	{
		m_Camera->SetPosition(
			m_DisplayPortal1->m_PosX + offsetFromPortal2.x,
			m_DisplayPortal1->m_PosY + offsetFromPortal2.y,
			m_DisplayPortal1->m_PosZ + offsetFromPortal2.z);

		camPos = m_Camera->GetPosition();
		XMFLOAT3 offsetFromPortal1 = XMFLOAT3(camPos.x - m_DisplayPortal1->m_PosX, camPos.y - m_DisplayPortal1->m_PosY, camPos.z - m_DisplayPortal1->m_PosZ);
		XMVECTOR offsetVector1 = XMLoadFloat3(&offsetFromPortal1);
		XMFLOAT3 offsetFromPortal2 = XMFLOAT3(camPos.x - m_DisplayPortal2->m_PosX, camPos.y - m_DisplayPortal2->m_PosY, camPos.z - m_DisplayPortal2->m_PosZ);
		XMVECTOR offsetVector2 = XMLoadFloat3(&offsetFromPortal2);
		m_previousPortalOffset1 = offsetVector1;
		m_previousPortalOffset2 = offsetVector2;

		return;
	}
}
