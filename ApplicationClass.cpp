#include "ApplicationClass.h"
#include "ModelParser.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Lights = 0;
	m_BitmapSpinner = 0;
	m_DirLight = 0;
	m_Timer = 0;
	m_Font = 0;
	m_TextString1 = 0;
	m_TextString2 = 0;
	m_Fps = 0;
	m_FpsString = 0;
	m_TextStringMouseBttn = 0;
	m_TextStringMouseX = 0;
	m_TextStringMouseY = 0;
	m_RenderTexDisplay = 0;
	m_DisplayPlane = 0;
	m_mountainGO = 0;
	m_IcosphereGO = 0;
	m_transIcoGO = 0;
	m_MadelineGO1 = 0;
	m_MadelineGO2 = 0;
	m_cubeGO = 0;
	m_RenderClass = 0;

	m_startTime = std::chrono::high_resolution_clock::now();

	//ModelParser::ParseFile("C:\\Users\\finnw\\OneDrive\\Documents\\3D objects\\Plane.obj");
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HWND hwnd)
{
	// GENERAL

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

	// PARAMETERS

	m_Parameters = new ShaderClass::ShaderParameters;
	m_Parameters->fog.fogStart = 0.0f;
	m_Parameters->fog.fogEnd = 40.0f;
	m_Parameters->clip.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 999999.0f);
	m_Parameters->textureTranslation.translation = XMFLOAT2(0, 0);
	m_Parameters->textureTranslation.timeMultiplier = 0.0f;
	m_Parameters->alpha.alphaBlend = 1.0f;
	m_Parameters->water.reflectRefractScale = 0.01f;

	m_Parameters->fire.distortion1 = XMFLOAT2(0.1f, 0.2f);
	m_Parameters->fire.distortion2 = XMFLOAT2(0.1f, 0.3f);
	m_Parameters->fire.distortion3 = XMFLOAT2(0.1f, 0.1f);
	m_Parameters->fire.distortionScale = 0.8f;
	m_Parameters->fire.distortionBias = 0.5f;

	XMVECTOR lightPos = XMVectorSet(DIR_LIGHT_X, DIR_LIGHT_Y, DIR_LIGHT_Z, 0);
	lightPos = XMVectorScale(lightPos, -SHADOW_MAP_DISTANCE);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));
	m_Parameters->shadow.shadowView = lightView;

	XMMATRIX lightProjection = XMMatrixOrthographicLH(SHADOW_MAP_SCENE_SIZE, SHADOW_MAP_SCENE_SIZE, SHADOW_MAP_NEAR, SHADOW_MAP_DEPTH);
	m_Parameters->shadow.shadowProj = lightProjection;

	m_Parameters->shadow.poissonDisk[0] = XMFLOAT2(-0.94201624, -0.39906216);
	m_Parameters->shadow.poissonDisk[1] = XMFLOAT2(0.94558609, -0.76890725);
	m_Parameters->shadow.poissonDisk[2] = XMFLOAT2(-0.094184101, -0.92938870);
	m_Parameters->shadow.poissonDisk[3] = XMFLOAT2(0.34495938, 0.29387760);
	

	// SHADERS	

	bool clampSamplerMode = true;
	ShaderClass* shaderMain = 0, * shaderReflect = 0, * shaderWater = 0, * shader2D = 0, * shaderFont = 0, * shaderFractal = 0, * shaderFire = 0, *shaderDepth = 0;
	result = 
		InitializeShader(hwnd, &shaderMain, "../GraphicsEngine/Fog.vs", "../GraphicsEngine/Fog.ps") &&
		InitializeShader(hwnd, &shaderReflect, "../GraphicsEngine/Reflect.vs", "../GraphicsEngine/Reflect.ps") &&
		InitializeShader(hwnd, &shaderWater, "../GraphicsEngine/Water.vs", "../GraphicsEngine/Water.ps") && 
		InitializeShader(hwnd, &shader2D, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/2D.ps") &&
		InitializeShader(hwnd, &shaderFont, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Font.ps") &&
		InitializeShader(hwnd, &shaderFire, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Fire.ps", clampSamplerMode) &&
		InitializeShader(hwnd, &shaderDepth, "../GraphicsEngine/Depth.vs", "../GraphicsEngine/Depth.ps") &&
		InitializeShader(hwnd, &shaderFractal, "../GraphicsEngine/Fog.vs", "../GraphicsEngine/Fractal.ps");
	if (!result)
		return false;

	m_RenderClass->SetDepthShader(shaderDepth);

	// TEXSETS

	m_TexSetMoss = new TextureSetClass;
	m_TexSetMoss->Add(device, deviceContext, "../GraphicsEngine/Data/Celeste.tga");
	m_TexSetMoss->Add(device, deviceContext, "../GraphicsEngine/Data/Moss.tga");
	m_TexSetMoss->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetMoss->Add(device, deviceContext, "../GraphicsEngine/Data/MossNormal.tga");

	m_TexSetStars = new TextureSetClass;
	m_TexSetStars->Add(device, deviceContext, "../GraphicsEngine/Data/Celeste.tga");
	m_TexSetStars->Add(device, deviceContext, "../GraphicsEngine/Data/Celeste.tga");
	m_TexSetStars->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetStars->Add(device, deviceContext, "../GraphicsEngine/Data/RockNormal.tga");

	m_TexSetSnow = new TextureSetClass;
	m_TexSetSnow->Add(device, deviceContext, "../GraphicsEngine/Data/Snow.tga");
	m_TexSetSnow->Add(device, deviceContext, "../GraphicsEngine/Data/Snow.tga");
	m_TexSetSnow->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetSnow->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultNormal.tga");

	m_TexSetReflection = new TextureSetClass;
	m_TexSetReflection->Add(device, deviceContext, "../GraphicsEngine/Data/Glass.tga");
	m_TexSetReflection->Add(device, deviceContext, "../GraphicsEngine/Data/Glass.tga");
	m_TexSetReflection->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetReflection->Add(device, deviceContext, "../GraphicsEngine/Data/GlassNormal.tga");

	m_TexSetWater = new TextureSetClass;
	m_TexSetWater->Add(device, deviceContext, "../GraphicsEngine/Data/WaterBlue.tga");
	m_TexSetWater->Add(device, deviceContext, "../GraphicsEngine/Data/WaterBlue.tga");
	m_TexSetWater->Add(device, deviceContext, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetWater->Add(device, deviceContext, "../GraphicsEngine/Data/WaterNormal.tga");

	m_TexSetNone = new TextureSetClass;

	auto texSetFire = new TextureSetClass;
	texSetFire->Add(device, deviceContext, "../GraphicsEngine/Data/NoiseAlt.tga");
	texSetFire->Add(device, deviceContext, "../GraphicsEngine/Data/fireColor.tga");
	texSetFire->Add(device, deviceContext, "../GraphicsEngine/Data/flameAlpha.tga");

	// MODELS

	ModelClass* modelMadeline =0, * modelMountain =0, * modelCube =0, * modelIcosphere =0, * modelPlane =0;
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

	InitializeGameObject(modelMadeline, shaderMain, m_TexSetMoss, "Madeline1", &m_MadelineGO1);
	InitializeGameObject(modelMadeline, shaderMain, m_TexSetStars, "Madeline2", &m_MadelineGO2);
	InitializeGameObject(modelIcosphere, shaderMain, m_TexSetMoss, "IcosphereBig", &m_IcosphereGO);
	InitializeGameObject(modelIcosphere, shaderMain, m_TexSetSnow, "IcosphereTrans", &m_transIcoGO);
	InitializeGameObject(modelMountain, shaderMain, m_TexSetSnow, "Mountain", &m_mountainGO);
	InitializeGameObject(modelPlane, shaderWater, m_TexSetWater, "Water", &waterGO);
	InitializeGameObject(modelCube, shaderReflect, m_TexSetReflection, "GlassCube", &m_cubeGO);
	InitializeGameObject(modelCube, shaderMain, m_TexSetMoss, "WaterCube", &waterCubeGO);
	InitializeGameObject(modelCube, shaderFractal, m_TexSetNone, "Fractal", &m_fractalGO);
	InitializeGameObject(modelPlane, shaderFire, texSetFire, "Fire", &fireGO);
	InitializeGameObject(modelPlane, shaderMain, m_TexSetSnow, "Floor", &floorGO);

	m_MadelineGO2->SetPosition(3, 0, 3);
	m_MadelineGO2->SetScale(0.5f, 0.5f, 0.5f);

	m_IcosphereGO->SetScale(5, 5, 5);
	m_IcosphereGO->SetPosition(0, 0, 15);

	m_transIcoGO->SetPosition(3, 0.5f, 3);
	m_transIcoGO->SetScale(1.0f, 1.0f, 1.0f);	

	m_mountainGO->SetScale(0.3f, 0.3f, 0.3f);
	m_mountainGO->SetPosition(1, -13, 15);

	m_cubeGO->SetScale(1.5f, 0.5f, 1.5f);
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

	m_numLights = 4;
	m_Lights = new LightClass[m_numLights];

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
	m_DirLight->SetDirection(DIR_LIGHT_X, DIR_LIGHT_Y, DIR_LIGHT_Z);
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
	m_TextString1->SetText("Cool 3D Graphics Project");
	m_TextString1->UpdateText();	

	m_TextString2->SetColor(0, 1, 0);
	m_TextString2->SetPosition(15, 30);
	m_TextString2->SetText("by Finn Wright");
	m_TextString2->UpdateText();	

	m_FpsString->SetPosition(10, 60);	

	m_TextStringMouseX->SetPosition(10, 80);
	m_TextStringMouseY->SetPosition(10, 100);
	m_TextStringMouseBttn->SetPosition(10, 120);

	// RENDER TEXTURES

	int texHeight = 256;
	int texWidth = 256;
	format = 1;

	m_RenderTexDisplay = new RenderTextureClass;
	result = m_RenderTexDisplay->Initialize(device, texWidth, texHeight, SCREEN_DEPTH, SCREEN_NEAR, format);
	if (!result)
		return false;

	texHeight = SHADOW_MAP_RENDER_X;
	texWidth = SHADOW_MAP_RENDER_Y;

	auto rendShadowMap = new RenderTextureClass;
	result = rendShadowMap->Initialize(device, texWidth, texHeight, SHADOW_MAP_DEPTH, SHADOW_MAP_NEAR, format);
	if (!result)
		return false;

	// DISPLAY PLANES

	int displayWidth = 1;
	int displayHeight = 1;

	m_DisplayPlane = new DisplayPlaneClass;
	result = m_DisplayPlane->Initialize(device, displayWidth, displayHeight, m_RenderTexDisplay, shader2D);
	if (!result)
		return false;
	m_DisplayPlane->SetPosition(0, 0, 5);

	m_RenderClass->AddDisplayPlane(m_DisplayPlane);

	auto shadowMapDisplay = new DisplayPlaneClass;
	result = shadowMapDisplay->Initialize(device, displayWidth, displayHeight, rendShadowMap, shader2D);
	if (!result)
		return false;
	shadowMapDisplay->SetPosition(0, 4, 0);

	m_RenderClass->SetShadowMapDisplayPlane(shadowMapDisplay);

	return true;
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

void ApplicationClass::InitializeGameObject(ModelClass* model, ShaderClass* shader, TextureSetClass* texSet, const char* name, GameObjectClass** ptr)
{
	*ptr = new GameObjectClass;

	(*ptr)->Initialize(model, shader, texSet, name);

	m_RenderClass->AddGameObject(*ptr);
}

void ApplicationClass::InitializeGameObject2D(BitmapClass* bitmap, ShaderClass* shader, GameObjectClass2D** ptr)
{
	*ptr = new GameObjectClass2D;
	(*ptr)->Initialize(bitmap, shader);

	m_RenderClass->AddGameObject2D(*ptr);
}

bool ApplicationClass::InitializeTextClass(TextClass** ptr, ShaderClass* shader, FontClass* font, int maxLength)
{
	*ptr = new TextClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), SCREEN_X, SCREEN_Y, maxLength, shader);
	if (!result)
		return false;

	(*ptr)->SetFont(font);

	m_RenderClass->AddTextClass(*ptr);

	return true;
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

	if (m_DisplayPlane)
	{
		m_DisplayPlane->Shutdown();
		delete m_DisplayPlane;
		m_DisplayPlane = 0;
	}

	if (m_RenderTexDisplay)
	{
		m_RenderTexDisplay->Shutdown();
		delete m_RenderTexDisplay;
		m_RenderTexDisplay = 0;
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

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool ApplicationClass::Frame(InputClass* Input)
{
	m_Timer->Frame();
	float frameTime = m_Timer->GetTime();

	m_BitmapSpinner->Update(frameTime);

	static float rotation = 0.0f;
	rotation -= 0.0174532925f;
	if (rotation < 0.0f)
		rotation += 360.0f;

	static float time = 0.0f;
	time += 0.1f;

	m_MadelineGO1->SetRotation(0, rotation * 10.0f, 0);
	m_IcosphereGO->SetRotation(rotation * 50.0f, 0, 0);
	m_fractalGO->SetRotation(0, rotation * sin(time * 0.01f) * 5, 0);
	
	if (Input->IsKeyPressed(DIK_ESCAPE))
		return false;

	int mouseX, mouseY;
	Input->GetMouseLocation(mouseX, mouseY);

	bool mouseDown = Input->IsMousePressed();
	if (!UpdateMouseStrings(mouseX, mouseY, mouseDown))
		return false;
	
	m_cursorGO2D->SetPosition(mouseX - 635, -mouseY + 400);

	m_Camera->Frame(Input, frameTime);

	m_Fps->Frame();

	return Render() && m_Fps->UpdateFPS(m_FpsString);
}

bool ApplicationClass::Render()
{
	bool result;

	for (int i = 0; i < m_numLights; i++)
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

	result = m_RenderClass->Render(m_Parameters);
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