#include "ApplicationClass.h"
#include "ModelParser.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Timer = 0;
	m_Font = 0;
	m_Fps = 0;
	m_RenderClass = 0;
	m_Frustum = 0;
	m_Parameters = 0;

	m_Lights = 0;
	m_DirLight = 0;

	m_BitmapSpinner = 0;
	m_BitmapCursor = 0;	
	
	m_TextString1 = 0;
	m_TextString2 = 0;	
	m_FpsString = 0;
	m_TextStringMouseBttn = 0;
	m_TextStringMouseX = 0;
	m_TextStringMouseY = 0;

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

	m_dirLightX = 0;
	m_dirLightY = 0;
	m_dirLightZ = 0;

	//ModelParser::ParseFile("C:\\Users\\finnw\\OneDrive\\Documents\\3D objects\\Plane.obj");
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HWND hwnd)
{
	// GENERAL

	m_Settings = new Settings(4);

	m_Direct3D = new D3DClass;	
	bool result = m_Direct3D->Initialize(SCREEN_X, SCREEN_Y, V_SYNC, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	ID3D11Device* device = m_Direct3D->GetDevice();
	ID3D11DeviceContext* deviceContext = m_Direct3D->GetDeviceContext();

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Initialize2DView();

	m_Timer = new TimerClass;
	result = m_Timer->Initialize();
	if (!result)
		return false;

	m_Frustum = new FrustumClass;

	m_Fps = new FpsClass();
	m_Fps->Initialize();

	m_RenderClass = new RenderClass;
	result = m_RenderClass->Initialize(m_Direct3D, m_Camera, m_Frustum);
	if (!result)
		return false;		

	// VARIABLES

	m_startTime = std::chrono::high_resolution_clock::now();
	m_dirLightX = -1.0f;
	m_dirLightY = -1.0f;
	m_dirLightZ = -1.0f;

	// PARAMETERS

	m_Parameters = new ShaderClass::ShaderParameters;
	UpdateParameters();

	// SHADERS	

	bool clampSamplerMode = true;
	ShaderClass* shaderMain = 0, * shaderReflect = 0, * shaderWater = 0, * shader2D = 0, * shaderFont = 0,
		* shaderFractal = 0, * shaderFire = 0, * shaderDepth = 0, * shaderBlur = 0, * shaderFilter = 0, * shaderSkybox = 0, * shaderPS =0,
		*shaderPortal =0;	
	result = 
		InitializeShader(hwnd, &shaderMain, "../GraphicsEngine/Fog.vs", "../GraphicsEngine/Fog.ps") &&
		InitializeShader(hwnd, &shaderReflect, "../GraphicsEngine/Reflect.vs", "../GraphicsEngine/Reflect.ps") &&
		InitializeShader(hwnd, &shaderWater, "../GraphicsEngine/Water.vs", "../GraphicsEngine/Water.ps") && 
		InitializeShader(hwnd, &shader2D, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/2D.ps") &&
		InitializeShader(hwnd, &shaderFont, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Font.ps") &&
		InitializeShader(hwnd, &shaderFire, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Fire.ps", clampSamplerMode) &&
		InitializeShader(hwnd, &shaderDepth, "../GraphicsEngine/Depth.vs", "../GraphicsEngine/Depth.ps") &&
		InitializeShader(hwnd, &shaderBlur, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Blur.ps") &&
		InitializeShader(hwnd, &shaderFilter, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Filter.ps") &&
		InitializeShader(hwnd, &shaderSkybox, "../GraphicsEngine/Skybox.vs", "../GraphicsEngine/Skybox.ps", clampSamplerMode) &&
		InitializeShader(hwnd, &shaderPS, "../GraphicsEngine/Particle.vs", "../GraphicsEngine/Particle.ps") &&
		InitializeShader(hwnd, &shaderPortal, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Portal.ps") &&
		InitializeShader(hwnd, &shaderFractal, "../GraphicsEngine/Fog.vs", "../GraphicsEngine/Fractal.ps");
	if (!result)
		return false;	

	m_RenderClass->SetDepthShader(shaderDepth);

	// TEXSETS

	TextureSetClass* texSetMoss, * texSetStars, * texSetSnow, * texSetReflection, * texSetWater, * texSetNone, * texSetFire, * texSetSkybox, * texSetPS;
	InitializeTexSet(&texSetMoss);
	InitializeTexSet(&texSetStars);
	InitializeTexSet(&texSetSnow);
	InitializeTexSet(&texSetReflection);
	InitializeTexSet(&texSetWater);
	InitializeTexSet(&texSetNone);
	InitializeTexSet(&texSetFire);
	InitializeTexSet(&texSetSkybox);
	InitializeTexSet(&texSetPS);

	texSetMoss->Add(device, deviceContext, "../GraphicsEngine/Data/Celeste.tga");
	texSetMoss->Add(device, deviceContext, "../GraphicsEngine/Data/Moss.tga");
	texSetMoss->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetMoss->Add(device, deviceContext, "../GraphicsEngine/Data/MossNormal.tga");

	texSetStars->Add(device, deviceContext, "../GraphicsEngine/Data/Celeste.tga");
	texSetStars->Add(device, deviceContext, "../GraphicsEngine/Data/Celeste.tga");
	texSetStars->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetStars->Add(device, deviceContext, "../GraphicsEngine/Data/RockNormal.tga");

	texSetSnow->Add(device, deviceContext, "../GraphicsEngine/Data/Snow.tga");
	texSetSnow->Add(device, deviceContext, "../GraphicsEngine/Data/Snow.tga");
	texSetSnow->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetSnow->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultNormal.tga");

	texSetReflection->Add(device, deviceContext, "../GraphicsEngine/Data/Glass.tga");
	texSetReflection->Add(device, deviceContext, "../GraphicsEngine/Data/Glass.tga");
	texSetReflection->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetReflection->Add(device, deviceContext, "../GraphicsEngine/Data/GlassNormal.tga");

	texSetWater->Add(device, deviceContext, "../GraphicsEngine/Data/WaterBlue.tga");
	texSetWater->Add(device, deviceContext, "../GraphicsEngine/Data/WaterBlue.tga");
	texSetWater->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	texSetWater->Add(device, deviceContext, "../GraphicsEngine/Data/WaterNormal.tga");

	texSetFire->Add(device, deviceContext, "../GraphicsEngine/Data/NoiseAlt.tga");
	texSetFire->Add(device, deviceContext, "../GraphicsEngine/Data/fireColor.tga");
	texSetFire->Add(device, deviceContext, "../GraphicsEngine/Data/flameAlpha.tga");

	result = texSetSkybox->AddCubemap(device, deviceContext, "../GraphicsEngine/Data/Skybox/Skybox.tga");
	if (!result)
		return false;

	texSetPS->Add(device, deviceContext, "../GraphicsEngine/Data/Sparkle.tga");

	// MODELS

	ModelClass* modelMadeline = 0, * modelMountain = 0, * modelCube = 0, * modelIcosphere = 0, * modelPlane = 0;
	result =
		InitializeModel(hwnd, &modelMadeline, "../GraphicsEngine/Models/Madeline.txt") &&
		InitializeModel(hwnd, &modelMountain, "../GraphicsEngine/Models/MountFuji.txt") &&
		InitializeModel(hwnd, &modelCube, "../GraphicsEngine/Models/Cube.txt") &&
		InitializeModel(hwnd, &modelIcosphere, "../GraphicsEngine/Models/Icosphere.txt") &&
		InitializeModel(hwnd, &modelPlane, "../GraphicsEngine/Models/Plane.txt");
	if (!result)
		return false;

	// BITMAPS

	result = 
		InitializeBitmap(&m_BitmapSpinner, "../GraphicsEngine/Animations/Spinner.txt") &&
		InitializeBitmap(&m_BitmapCursor, "../GraphicsEngine/Animations/MouseAnim.txt");	
	if (!result)
		return false;

	// GAMEOBJECTS

	GameObjectClass* waterGO = 0, * waterCubeGO = 0, * fireGO, * floorGO;
	float waterSize = 2;
	bool opaque = false, transparent = true;

	InitializeGameObject(modelMadeline, shaderMain, texSetMoss, opaque, "Madeline1", &m_MadelineGO1);
	InitializeGameObject(modelMadeline, shaderMain, texSetStars, opaque, "Madeline2", &m_MadelineGO2);
	InitializeGameObject(modelIcosphere, shaderMain, texSetMoss, opaque, "IcosphereBig", &m_IcosphereGO);
	InitializeGameObject(modelIcosphere, shaderMain, texSetSnow, transparent, "IcosphereTrans", &m_transIcoGO);
	InitializeGameObject(modelMountain, shaderMain, texSetSnow, opaque, "Mountain", &m_mountainGO);
	InitializeGameObject(modelPlane, shaderWater, texSetWater, transparent, "Water", &waterGO);
	InitializeGameObject(modelCube, shaderReflect, texSetReflection, opaque, "GlassCube", &m_cubeGO);
	InitializeGameObject(modelCube, shaderMain, texSetMoss, opaque, "WaterCube", &waterCubeGO);
	InitializeGameObject(modelCube, shaderFractal, texSetNone, opaque, "Fractal", &m_fractalGO);
	InitializeGameObject(modelPlane, shaderFire, texSetFire, transparent, "Fire", &fireGO);
	InitializeGameObject(modelPlane, shaderMain, texSetSnow, opaque, "Floor", &floorGO);
	InitializeGameObject(modelCube, shaderSkybox, texSetSkybox, opaque, "Skybox", &m_skyboxGO);
	InitializeGameObject(modelIcosphere, shaderMain, texSetSnow, opaque, "Test", &m_testIcoGO);

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

	m_fractalGO->SetPosition(25,0,0);
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
	m_RenderClass->SubscribeToReflection(device, m_cubeGO, texSetIndex, format);
	m_RenderClass->SubscribeToReflection(device, waterGO, texSetIndex, format);

	texSetIndex = 5;
	m_RenderClass->SubscribeToRefraction(device, waterGO, texSetIndex, format);

	texSetIndex = 4;
	m_RenderClass->SubscribeToShadow(m_MadelineGO1, texSetIndex);
	m_RenderClass->SubscribeToShadow(floorGO, texSetIndex);
	m_RenderClass->SubscribeToShadow(m_IcosphereGO, texSetIndex);
	m_RenderClass->SubscribeToShadow(m_transIcoGO, texSetIndex);
	m_RenderClass->SubscribeToShadow(m_mountainGO, texSetIndex);
	m_RenderClass->SubscribeToShadow(waterCubeGO, texSetIndex);

	// 2D GAMEOBJECTS

	InitializeGameObject2D(m_BitmapSpinner, shader2D, &m_spinnerGO2D);
	InitializeGameObject2D(m_BitmapCursor, shader2D, &m_cursorGO2D);
	
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
	m_DirLight->SetDirection(m_dirLightX, m_dirLightY, m_dirLightZ);
	m_DirLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLight->SetSpecularPower(32.0f);

	// FONTS

	m_Font = new FontClass;
	result = m_Font->Initialize(device, deviceContext, 0);
	if (!result)
		return false;

	// TEXT

	int maxLength = 32;
	result =
		InitializeTextClass(&m_TextString1, shaderFont, m_Font, maxLength) &&
		InitializeTextClass(&m_TextString2, shaderFont, m_Font, maxLength) &&
		InitializeTextClass(&m_FpsString, shaderFont, m_Font, maxLength) &&
		InitializeTextClass(&m_TextStringMouseX, shaderFont, m_Font, maxLength) &&
		InitializeTextClass(&m_TextStringMouseY, shaderFont, m_Font, maxLength) &&
		InitializeTextClass(&m_TextStringMouseBttn, shaderFont, m_Font, maxLength);
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

	m_FpsString->SetPosition(10, 60);	

	m_TextStringMouseX->SetPosition(10, 80);
	m_TextStringMouseY->SetPosition(10, 100);
	m_TextStringMouseBttn->SetPosition(10, 120);

	// RENDER TEXTURES

	int texHeight = 256;
	int texWidth = 256;
	format = 1;

	int portalResolution = 1024;

	auto rendMainDisplay = new RenderTextureClass;
	auto rendPortal1 = new RenderTextureClass;
	auto rendPortal2 = new RenderTextureClass;
	result = rendMainDisplay->Initialize(device, texWidth, texHeight, SCREEN_DEPTH, SCREEN_NEAR, format) &&
		rendPortal1->Initialize(device, portalResolution, portalResolution, SCREEN_DEPTH, SCREEN_NEAR, format) &&
		rendPortal2->Initialize(device, portalResolution, portalResolution, SCREEN_DEPTH, SCREEN_NEAR, format);
	if (!result)
		return false;

	texHeight = m_Settings->m_CurrentData.ShadowMapRenderX;
	texWidth = m_Settings->m_CurrentData.ShadowMapRenderY;

	auto rendShadowMap = new RenderTextureClass;
	result = rendShadowMap->Initialize(device, texWidth, texHeight, m_Settings->m_CurrentData.ShadowMapDepth, m_Settings->m_CurrentData.ShadowMapNear, format);
	if (!result)
		return false;

	texHeight = (int)(SCREEN_X * m_Settings->m_CurrentData.DownScaleMult);
	texWidth = (int)(SCREEN_Y * m_Settings->m_CurrentData.DownScaleMult);

	auto rendPostProcessing = new RenderTextureClass;
	auto rendPostProcessing2 = new RenderTextureClass;
	auto rendPostProcessing3 = new RenderTextureClass;
	result = rendPostProcessing->Initialize(device, texWidth, texHeight, SCREEN_DEPTH, SCREEN_NEAR, format) &&
		rendPostProcessing2->Initialize(device, texWidth, texHeight, SCREEN_DEPTH, SCREEN_NEAR, format) && 
		rendPostProcessing3->Initialize(device, texWidth, texHeight, SCREEN_DEPTH, SCREEN_NEAR, format);
	if (!result)
		return false;		

	m_RendTexList.push_back(rendMainDisplay);
	m_RendTexList.push_back(rendShadowMap);
	m_RendTexList.push_back(rendPostProcessing);
	m_RendTexList.push_back(rendPostProcessing2);
	m_RendTexList.push_back(rendPostProcessing3);
	m_RendTexList.push_back(rendPortal1);
	m_RendTexList.push_back(rendPortal2);

	// DISPLAY PLANES

	float displayWidth = 1.0f;
	float displayHeight = 1.0f;

	m_DisplayPlane = new DisplayPlaneClass;
	result = m_DisplayPlane->Initialize(device, displayWidth, displayHeight, rendMainDisplay, shader2D, "MainDisplay", m_Camera);
	if (!result)
		return false;
	m_DisplayPlane->SetPosition(0, 0, 5);	

	auto shadowMapDisplay = new DisplayPlaneClass;
	result = shadowMapDisplay->Initialize(device, displayWidth, displayHeight, rendShadowMap, shader2D, "ShadowMap");
	if (!result)
		return false;
	shadowMapDisplay->SetPosition(0, 4, 0);

	auto displayPostProcessing = new DisplayPlaneClass;
	result = displayPostProcessing->Initialize(device, SCREEN_X, SCREEN_Y, rendPostProcessing, shaderBlur, "DisplayPP1");
	if (!result)
		return false;

	auto displayPostProcessing2 = new DisplayPlaneClass;
	result = displayPostProcessing2->Initialize(device, SCREEN_X, SCREEN_Y, rendPostProcessing2, shaderBlur, "DisplayPP2");
	if (!result)
		return false;

	auto displayPostProcessing3 = new DisplayPlaneClass;
	result = displayPostProcessing3->Initialize(device, SCREEN_X, SCREEN_Y, rendPostProcessing3, shaderFilter, "DisplayPP3");
	if (!result)
		return false;

	m_DisplayPortal1 = new DisplayPlaneClass;
	result = m_DisplayPortal1->Initialize(device, displayWidth, displayHeight, rendPortal1, shaderPortal, "Portal1");
	if (!result)
		return false;

	m_DisplayPortal2 = new DisplayPlaneClass;
	result = m_DisplayPortal2->Initialize(device, displayWidth, displayHeight, rendPortal2, shaderPortal, "Portal2");
	if (!result)
		return false;

	float displaySize = 0.5f;
	displayPostProcessing->SetScale(displaySize, displaySize, displaySize);
	displayPostProcessing2->SetScale(displaySize, displaySize, displaySize);
	displayPostProcessing3->SetScale(displaySize, displaySize, displaySize);

	m_DisplayPortal1->SetPosition(8, 0.0f, 0);
	m_DisplayPortal1->SetRotation(0, 0, 0);
	m_DisplayPortal1->SetScale(2);

	m_DisplayPortal2->SetPosition(-8, 0, 0);
	m_DisplayPortal2->SetRotation(0, 0, 0);
	m_DisplayPortal2->SetScale(2);

	m_RenderClass->SetShadowMapDisplayPlane(shadowMapDisplay);
	m_RenderClass->SetPostProcessingDisplayPlanes(displayPostProcessing, displayPostProcessing2, displayPostProcessing3);

	m_RenderClass->AddDisplayPlane(m_DisplayPlane);
	m_RenderClass->AddDisplayPlane(m_DisplayPortal1);
	m_RenderClass->AddDisplayPlane(m_DisplayPortal2);

	m_DisplayList.push_back(shadowMapDisplay);
	m_DisplayList.push_back(m_DisplayPlane);
	m_DisplayList.push_back(displayPostProcessing);
	m_DisplayList.push_back(displayPostProcessing2);
	m_DisplayList.push_back(displayPostProcessing3);
	m_DisplayList.push_back(m_DisplayPortal1);
	m_DisplayList.push_back(m_DisplayPortal2);

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

	result = InitializeParticleSystem(&m_PSRaindrops, data, shaderPS, texSetPS, "PS1");
	if (!result)
		return false;

	return true;
}

void ApplicationClass::InitializeShadowMapViewMatrix()
{
	XMVECTOR lightPos = XMVectorSet(m_dirLightX, m_dirLightY, m_dirLightZ, 0);
	lightPos = XMVectorScale(lightPos, -m_Settings->m_CurrentData.ShadowMapDistance);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));
	m_Parameters->shadow.shadowView = lightView;
}

bool ApplicationClass::InitializeModel(HWND hwnd, ModelClass** ptr, const char* name)
{
	char modelFilename[128];
	strcpy_s(modelFilename, name);

	*ptr = new ModelClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_ModelList.push_back(*ptr);

	return true;
}

bool ApplicationClass::InitializeShader(HWND hwnd, ShaderClass** ptr, const char* vertexName, const char* fragName, bool clamp)
{
	char vertexShader[128], fragShader[128];

	strcpy_s(vertexShader, vertexName);
	strcpy_s(fragShader, fragName);

	*ptr = new ShaderClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader, clamp);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_ShaderList.push_back(*ptr);

	return true;
}

void ApplicationClass::InitializeGameObject(ModelClass* model, ShaderClass* shader, TextureSetClass* texSet, bool transparent, const char* name, GameObjectClass** ptr)
{
	*ptr = new GameObjectClass;

	(*ptr)->Initialize(model, shader, texSet, name);

	m_RenderClass->AddGameObject(*ptr, transparent);
	m_GameobjectList.push_back(*ptr);
}

void ApplicationClass::InitializeGameObject2D(BitmapClass* bitmap, ShaderClass* shader, GameObjectClass2D** ptr)
{
	*ptr = new GameObjectClass2D;
	(*ptr)->Initialize(bitmap, shader);

	m_RenderClass->AddGameObject2D(*ptr);
	m_Gameobject2DList.push_back(*ptr);
}

bool ApplicationClass::InitializeTextClass(TextClass** ptr, ShaderClass* shader, FontClass* font, int maxLength)
{
	*ptr = new TextClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), SCREEN_X, SCREEN_Y, maxLength, shader);
	if (!result)
		return false;

	(*ptr)->SetFont(font);

	m_RenderClass->AddTextClass(*ptr);
	m_TextList.push_back(*ptr);

	return true;
}

void ApplicationClass::InitializeTexSet(TextureSetClass** ptr)
{
	*ptr = new TextureSetClass;

	m_TexSetList.push_back(*ptr);
}

bool ApplicationClass::InitializeBitmap(BitmapClass** ptr, const char* filename)
{
	char bitmapFilename[128];

	strcpy_s(bitmapFilename, filename);
	*ptr = new BitmapClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), SCREEN_X, SCREEN_Y, bitmapFilename);
	if (!result)
		return false;

	m_BitmapList.push_back(*ptr);

	return true;
}

bool ApplicationClass::InitializeParticleSystem(ParticleSystemClass** ptr, ParticleSystemClass::ParticleSystemData data, ShaderClass* shader, TextureSetClass* texSet, const char* name)
{
	*ptr = new ParticleSystemClass();
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), data, shader, texSet, name);
	if (!result)
		return false;

	m_ParticleSystemList.push_back(*ptr);
	m_RenderClass->AddParticleSystem(*ptr);

	return true;
}

void ApplicationClass::UpdateParameters()
{
	m_Parameters->utils.texelSizeX = 1.0f / (SCREEN_X * m_Settings->m_CurrentData.DownScaleMult);
	m_Parameters->utils.texelSizeY = 1.0f / (SCREEN_Y * m_Settings->m_CurrentData.DownScaleMult);
	
	if (!m_Settings->m_CurrentData.FogEnabled)
	{
		m_Parameters->fog.fogStart = -9999999.0f;
		m_Parameters->fog.fogEnd = 999999999.0f;
	}
	else
	{
		m_Parameters->fog.fogStart = 0.0f;
		m_Parameters->fog.fogEnd = 20.0f;
	}

	m_Parameters->clip.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 999999.0f);
	m_Parameters->textureTranslation.translation = XMFLOAT2(0, 0);
	m_Parameters->textureTranslation.timeMultiplier = 0.0f;
	m_Parameters->alpha.alphaBlend = 1.0f;
	m_Parameters->water.reflectRefractScale = 0.01f;
	m_Parameters->reflectionEnabled = m_Settings->m_CurrentData.ReflectionEnabled;

	m_Parameters->fire.distortion1 = XMFLOAT2(0.1f, 0.2f);
	m_Parameters->fire.distortion2 = XMFLOAT2(0.1f, 0.3f);
	m_Parameters->fire.distortion3 = XMFLOAT2(0.1f, 0.1f);
	m_Parameters->fire.distortionScale = 0.8f;
	m_Parameters->fire.distortionBias = 0.5f;

	InitializeShadowMapViewMatrix();
	XMMATRIX lightProjection = XMMatrixOrthographicLH(m_Settings->m_CurrentData.ShadowMapSceneSize, m_Settings->m_CurrentData.ShadowMapSceneSize,
		m_Settings->m_CurrentData.ShadowMapNear, m_Settings->m_CurrentData.ShadowMapDepth);

	m_Parameters->shadow.shadowProj = lightProjection;
	m_Parameters->shadow.poissonDisk[0] = XMFLOAT4(-0.94201624f, -0.39906216f, 0, 0);
	m_Parameters->shadow.poissonDisk[1] = XMFLOAT4(0.94558609f, -0.76890725f, 0, 0);
	m_Parameters->shadow.poissonDisk[2] = XMFLOAT4(-0.094184101f, -0.92938870f, 0, 0);
	m_Parameters->shadow.poissonDisk[3] = XMFLOAT4(0.34495938f, 0.29387760f, 0, 0);
	m_Parameters->shadow.poissonSpread = 4096;
	m_Parameters->shadow.shadowBias = 0.005f;
	m_Parameters->shadow.shadowCutOff = 0.01f;

	m_Parameters->blur.blurMode = 0;

	m_Parameters->blur.weights[0] = XMFLOAT4(1.0f, 0, 0, 0);
	m_Parameters->blur.weights[1] = XMFLOAT4(0.9f, 0, 0, 0);
	m_Parameters->blur.weights[2] = XMFLOAT4(0.55f, 0, 0, 0);
	m_Parameters->blur.weights[3] = XMFLOAT4(0.18f, 0, 0, 0);

	if (m_Settings->m_CurrentData.FiltersEnabled)
	{
		m_Parameters->filter.grainEnabled = true;
		m_Parameters->filter.monochromeEnabled = true;
		m_Parameters->filter.sharpnessEnabled = false;
		m_Parameters->filter.vignetteEnabled = true;

		m_Parameters->filter.grainIntensity = 0.1f;
		m_Parameters->filter.vignetteStrength = 2;
		m_Parameters->filter.vignetteSmoothness = 0.6f;

		m_Parameters->filter.sharpnessKernalN = -0.5f;
		m_Parameters->filter.sharpnessKernalP = 0.5f;
		m_Parameters->filter.sharpnessStrength = 0.2f;
	}
}

void ApplicationClass::Shutdown()
{
	if (m_RenderClass)
	{
		m_RenderClass->Shutdown();
		delete m_RenderClass;
		m_RenderClass = 0;
	}

	for (auto model : m_ModelList)
	{
		model->Shutdown();
		delete model;
	}

	for (auto shader : m_ShaderList)
	{
		shader->Shutdown();
		delete shader;
	}	

	for (auto bitmap : m_BitmapList)
	{
		bitmap->Shutdown();
		delete bitmap;
	}

	for (auto texSet : m_TexSetList)
	{
		texSet->Shutdown();
		delete texSet;
	}

	for (auto display : m_DisplayList)
	{
		display->Shutdown();
		delete display;
	}

	for (auto rendTex : m_RendTexList)
	{
		rendTex->Shutdown();
		delete rendTex;
	}

	for (auto go : m_GameobjectList)
	{
		go->Shutdown();
		delete go;
	}

	for (auto go : m_Gameobject2DList)
	{
		go->Shutdown();
		delete go;
	}

	for (auto txt : m_TextList)
	{
		txt->Shutdown();
		delete txt;
	}

	for (auto ps : m_ParticleSystemList)
	{
		ps->Shutdown();
		delete ps;
	}

	if (m_Parameters)
	{
		delete m_Parameters;
		m_Parameters = 0;
	}

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}	

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

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_Frustum)
	{
		m_Frustum->Shutdown();
		delete m_Frustum;
		m_Frustum = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool ApplicationClass::Frame(InputClass* input)
{
	int outSettingsIndex;
	if (input->IsNumberPressed(outSettingsIndex))
	{
		m_Settings->ChangeSettings(outSettingsIndex);
		UpdateParameters();
	}

	m_Timer->Frame();
	float frameTime = m_Timer->GetTime();

	XMFLOAT3 camPos = m_Camera->GetPosition();
	XMFLOAT3 camRot = m_Camera->GetRotation();

	m_BitmapSpinner->Update(frameTime);

	static float rotation = 0.0f;
	rotation -= 0.0174532925f;
	if (rotation < 0.0f)
		rotation += 360.0f;

	static float time = 0.0f;
	time += 0.1f;

	if (m_MadelineGO1)
		m_MadelineGO1->SetRotation(0, rotation * 10.0f, 0);
	if (m_IcosphereGO)
		m_IcosphereGO->SetRotation(rotation * 50.0f, 0, 0);
	if (m_fractalGO)
		m_fractalGO->SetRotation(0, rotation * sin(time * 0.01f) * 5, 0);
	
	if (m_skyboxGO && !m_Settings->m_CurrentData.FreezeSkybox)
		m_skyboxGO->SetPosition(camPos.x, camPos.y, camPos.z);
	if (m_skyboxGO)
		m_skyboxGO->SetScale(m_Settings->m_CurrentData.ShadowsEnabled ? 500 : 0);

	float sunSpeed = 0.005f;
	SetDirLight(cos(time * sunSpeed), -0.75f, sin(time * sunSpeed));
	
	if (input->IsKeyPressed(DIK_ESCAPE))
		return false;

	int mouseX, mouseY;
	input->GetMouseLocation(mouseX, mouseY);

	bool mouseDown = input->IsMousePressed();
	if (!UpdateMouseStrings(mouseX, mouseY, mouseDown))
		return false;
	
	m_cursorGO2D->SetPosition(mouseX - 635.0f, -mouseY + 400.0f);

	m_Fps->Frame();

	if (m_PSRaindrops)
	{
		m_PSRaindrops->m_data.posX = camPos.x;
		m_PSRaindrops->m_data.posY = camPos.y + 20;
		m_PSRaindrops->m_data.posZ = camPos.z;
	}

	for (auto ps : m_ParticleSystemList)
	{
		ps->m_disabled = !m_Settings->m_CurrentData.ParticlesEnabled;

		bool result = ps->Frame(m_Direct3D->GetDeviceContext(), frameTime);
		if (!result)
			return false;
	}

	UpdatePortals(camPos);

	return Render() && m_Fps->UpdateFPS(m_FpsString) && LateFrame(input, frameTime);
}

bool ApplicationClass::LateFrame(InputClass* input, float frameTime)
{
	m_Camera->Frame(input, frameTime, m_Settings->m_CurrentData.CameraSpeed, m_Settings->m_CurrentData.CameraRotationSpeed);

	return true;
}

void ApplicationClass::SetDirLight(float x, float y, float z)
{
	m_dirLightX = x;
	m_dirLightY = y;
	m_dirLightZ = z;

	InitializeShadowMapViewMatrix();
	m_DirLight->SetDirection(x, y, z);
}

bool ApplicationClass::Render()
{
	bool result;

	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		m_Parameters->lightColor.diffuseColor[i] = m_Lights[i].GetDiffuseColor();
		m_Parameters->lightPos.lightPosition[i] = m_Lights[i].GetPosition();
	}

	m_Parameters->light.ambientColor = m_DirLight->GetAmbientColor();
	m_Parameters->light.diffuseColor = m_DirLight->GetDiffuseColor();
	m_Parameters->light.lightDirection = m_DirLight->GetDirection();
	m_Parameters->light.specularColor = m_DirLight->GetSpecularColor();
	m_Parameters->light.specularPower = m_DirLight->GetSpecularPower();

	auto now = std::chrono::high_resolution_clock::now();
	m_Parameters->utils.time = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_startTime).count();

	m_Parameters->camera.cameraPosition = m_Camera->GetPosition();	

	result = m_RenderClass->Render(m_Settings, m_Parameters);
	if (!result)
		return false;
	
	return true;
}

bool ApplicationClass::UpdateMouseStrings(int mouseX, int mouseY, bool mouseDown)
{
	char str[32];

	sprintf_s(str, "Mouse X: %d", mouseX);
	m_TextStringMouseX->SetText(str);

	sprintf_s(str, "Mouse Y: %d", mouseY);	
	m_TextStringMouseY->SetText(str);

	sprintf_s(str, sizeof(str), "Mouse Button: %s", (mouseDown ? "Yes" : "No"));
	m_TextStringMouseBttn->SetText(str);	

	return m_TextStringMouseX->UpdateText() && m_TextStringMouseY->UpdateText() && m_TextStringMouseBttn->UpdateText();
}

void ApplicationClass::UpdatePortals(XMFLOAT3 camPos)
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
