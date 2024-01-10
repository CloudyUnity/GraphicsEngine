#include "ApplicationClass.h"
#include "ModelParser.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_ModelMadeline = 0;
	m_ShaderMain = 0;
	m_Lights = 0;
	m_Bitmap = 0;
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
	m_ModelIcosphere = 0;
	m_ModelCube = 0;
	m_ModelMountain = 0;
	m_cubeGO = 0;
	m_ShaderReflect = 0;
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

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char text[128];
	char bitmapFilename[128];
	char vertexShader[128], fragShader[128];

	m_Direct3D = new D3DClass;	
	bool result = m_Direct3D->Initialize(screenWidth, screenHeight, V_SYNC, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Initialize2DView();

	m_Timer = new TimerClass;
	result = m_Timer->Initialize();
	if (!result)
		return false;

	m_Frustum = new FrustumClass;

	m_RenderClass = new RenderClass;
	result = m_RenderClass->Initialize(m_Direct3D, m_Camera, m_Frustum);
	if (!result)
		return false;

	strcpy_s(vertexShader, "../GraphicsEngine/Fog.vs");
	strcpy_s(fragShader, "../GraphicsEngine/Fog.ps");
	m_ShaderMain = new ShaderClass;
	result = m_ShaderMain->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(vertexShader, "../GraphicsEngine/Reflect.vs");
	strcpy_s(fragShader, "../GraphicsEngine/Reflect.ps");
	m_ShaderReflect = new ShaderClass;
	result = m_ShaderReflect->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(vertexShader, "../GraphicsEngine/Water.vs");
	strcpy_s(fragShader, "../GraphicsEngine/Water.ps");
	m_ShaderWater = new ShaderClass;
	result = m_ShaderWater->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(vertexShader, "../GraphicsEngine/Simple.vs");
	strcpy_s(fragShader, "../GraphicsEngine/2D.ps");
	m_Shader2D = new ShaderClass;
	result = m_Shader2D->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(vertexShader, "../GraphicsEngine/Simple.vs");
	strcpy_s(fragShader, "../GraphicsEngine/Font.ps");
	m_ShaderFont = new ShaderClass;
	result = m_ShaderFont->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_TexSetMoss = new TextureSetClass;
	m_TexSetMoss->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/Celeste.tga");
	m_TexSetMoss->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/Moss.tga");
	m_TexSetMoss->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetMoss->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/MossNormal.tga");

	m_TexSetStars = new TextureSetClass;
	m_TexSetStars->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/Celeste.tga");
	m_TexSetStars->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/Celeste.tga");
	m_TexSetStars->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetStars->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/RockNormal.tga");

	m_TexSetSnow = new TextureSetClass;
	m_TexSetSnow->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/Snow.tga");
	m_TexSetSnow->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/Snow.tga");
	m_TexSetSnow->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetSnow->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/DefaultNormal.tga");

	m_TexSetReflection = new TextureSetClass;
	m_TexSetReflection->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/Glass.tga");
	m_TexSetReflection->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/Glass.tga");
	m_TexSetReflection->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetReflection->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/GlassNormal.tga");

	m_TexSetWater = new TextureSetClass;
	m_TexSetWater->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/WaterBlue.tga");
	m_TexSetWater->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/WaterBlue.tga");
	m_TexSetWater->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	m_TexSetWater->Add(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../GraphicsEngine/Data/WaterNormal.tga");

	result = 
		InitializeModel(hwnd, &m_ModelMadeline, "../GraphicsEngine/Models/Madeline.txt") &&
		InitializeModel(hwnd, &m_ModelMountain, "../GraphicsEngine/Models/MountFuji.txt") &&
		InitializeModel(hwnd, &m_ModelCube, "../GraphicsEngine/Models/Cube.txt") &&
		InitializeModel(hwnd, &m_ModelIcosphere, "../GraphicsEngine/Models/Icosphere.txt") &&
		InitializeModel(hwnd, &m_ModelWater, "../GraphicsEngine/Models/Plane.txt");
	if (!result)
		return false;

	m_MadelineGO1 = new GameObjectClass;
	m_MadelineGO1->Initialize(m_ModelMadeline, m_ShaderMain, m_TexSetMoss, "Madeline1");

	m_MadelineGO2 = new GameObjectClass;
	m_MadelineGO2->Initialize(m_ModelMadeline, m_ShaderMain, m_TexSetStars, "Madeline2");
	m_MadelineGO2->SetPosition(3, 0, 3);
	m_MadelineGO2->SetScale(0.5f, 0.5f, 0.5f);

	m_IcosphereGO = new GameObjectClass;
	m_IcosphereGO->Initialize(m_ModelIcosphere, m_ShaderMain, m_TexSetMoss, "IcosphereBig");
	m_IcosphereGO->SetScale(5, 5, 5);
	m_IcosphereGO->SetPosition(0, 0, 15);

	m_transIcoGO = new GameObjectClass;
	m_transIcoGO->Initialize(m_ModelIcosphere, m_ShaderMain, m_TexSetSnow, "IcosphereTrans");
	m_transIcoGO->SetPosition(3, 0.5f, 3);
	m_transIcoGO->SetScale(1.0f, 1.0f, 1.0f);	

	m_mountainGO = new GameObjectClass;
	m_mountainGO->Initialize(m_ModelMountain, m_ShaderMain, m_TexSetSnow, "Mountain");
	m_mountainGO->SetScale(0.3f, 0.3f, 0.3f);
	m_mountainGO->SetPosition(1, -13, 15);

	m_cubeGO = new GameObjectClass;
	m_cubeGO->Initialize(m_ModelCube, m_ShaderReflect, m_TexSetReflection, "GlassCube");
	m_cubeGO->SetScale(1.5f, 0.5f, 1.5f);
	m_cubeGO->SetPosition(0, -0.5f, 0);

	float waterSize = 2;
	GameObjectClass* waterGO = new GameObjectClass;
	waterGO->Initialize(m_ModelWater, m_ShaderWater, m_TexSetWater, "Water");
	waterGO->SetPosition(6, 0.5f, -0.5f);
	waterGO->SetScale(waterSize, 0.000000000001f, waterSize);

	auto waterCube = new GameObjectClass;
	waterCube->Initialize(m_ModelMadeline, m_ShaderMain, m_TexSetMoss, "WaterCube");
	waterCube->SetScale(0.5f, 0.5f, 0.5f);
	waterCube->SetPosition(6, 0, -0.5f);

	m_RenderClass->AddGameObject(m_MadelineGO1);
	m_RenderClass->AddGameObject(m_MadelineGO2);
	m_RenderClass->AddGameObject(m_IcosphereGO);
	m_RenderClass->AddGameObject(m_transIcoGO);
	m_RenderClass->AddGameObject(m_mountainGO);
	m_RenderClass->AddGameObject(m_cubeGO);
	m_RenderClass->AddGameObject(waterGO);
	m_RenderClass->AddGameObject(waterCube);

	int texSetIndex = 4;
	int format = 1;
	m_RenderClass->SubscribeToReflection(m_Direct3D->GetDevice(), m_cubeGO, texSetIndex, format);
	m_RenderClass->SubscribeToReflection(m_Direct3D->GetDevice(), waterGO, texSetIndex, format);
	texSetIndex = 5;
	m_RenderClass->SubscribeToRefraction(m_Direct3D->GetDevice(), waterGO, texSetIndex, format);

	strcpy_s(bitmapFilename, "../GraphicsEngine/Animations/Spinner.txt");
	m_Bitmap = new BitmapClass;
	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename);
	if (!result)
		return false;
	m_SpinnerObj = new GameObjectClass2D;
	m_SpinnerObj->Initialize(m_Bitmap, m_Shader2D);
	m_SpinnerObj->SetPosition(1000, 50);
	m_SpinnerObj->SetScale(0.75f, 0.75f);

	strcpy_s(bitmapFilename, "../GraphicsEngine/Animations/MouseAnim.txt");
	m_MouseCursor = new BitmapClass;
	result = m_MouseCursor->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename);
	if (!result)
		return false;
	m_MouseObj = new GameObjectClass2D;
	m_MouseObj->Initialize(m_MouseCursor, m_Shader2D);
	m_MouseObj->SetScale(0.1f, 0.1f);
	m_MouseObj->SetRotation(225);

	m_RenderClass->AddGameObject2D(m_SpinnerObj);
	m_RenderClass->AddGameObject2D(m_MouseObj);

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
	m_DirLight->SetDirection(-1.0f, 0.0f, -1.0f);
	m_DirLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLight->SetSpecularPower(32.0f);

	m_Font = new FontClass;
	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if (!result)
		return false;
	
	strcpy_s(text, "Sample Text 1");
	m_TextString1 = new TextClass;
	int maxLength = 32;
	result = m_TextString1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_ShaderFont);
	if (!result)
		return false;
	m_TextString1->SetColor(1, 0, 0);
	m_TextString1->SetFont(m_Font);
	m_TextString1->SetPosition(10, 10);
	m_TextString1->SetText(text);
	m_TextString1->UpdateText();	

	strcpy_s(text, "Sample Text 2");
	m_TextString2 = new TextClass;
	result = m_TextString2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_ShaderFont);
	if (!result)
		return false;
	m_TextString2->SetColor(0, 1, 0);
	m_TextString2->SetFont(m_Font);
	m_TextString2->SetPosition(15, 30);
	m_TextString2->SetText(text);
	m_TextString2->UpdateText();	

	m_Fps = new FpsClass();
	m_Fps->Initialize();
	m_previousFps = -1;

	m_FpsString = new TextClass;
	result = m_FpsString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_ShaderFont);
	if (!result)
		return false;
	m_FpsString->SetFont(m_Font);
	m_FpsString->SetPosition(10, 60);	

	m_TextStringMouseX = new TextClass;
	m_TextStringMouseY = new TextClass;
	m_TextStringMouseBttn = new TextClass;
	result = m_TextStringMouseX->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_ShaderFont);
	if (!result)
		return false;
	result = m_TextStringMouseY->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_ShaderFont);
	if (!result)
		return false;
	result = m_TextStringMouseBttn->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_ShaderFont);
	if (!result)
		return false;	
	m_TextStringMouseX->SetFont(m_Font);
	m_TextStringMouseY->SetFont(m_Font);
	m_TextStringMouseBttn->SetFont(m_Font);
	m_TextStringMouseX->SetPosition(10, 80);
	m_TextStringMouseY->SetPosition(10, 100);
	m_TextStringMouseBttn->SetPosition(10, 120);

	m_RenderClass->AddTextClass(m_TextString1);
	m_RenderClass->AddTextClass(m_TextString2);
	m_RenderClass->AddTextClass(m_FpsString);
	m_RenderClass->AddTextClass(m_TextStringMouseX);
	m_RenderClass->AddTextClass(m_TextStringMouseY);
	m_RenderClass->AddTextClass(m_TextStringMouseBttn);

	int texHeight = 256;
	int texWidth = 256;

	int displayWidth = 1;
	int displayHeight = 1;

	m_RenderTexDisplay = new RenderTextureClass;
	result = m_RenderTexDisplay->Initialize(m_Direct3D->GetDevice(), texWidth, texHeight, SCREEN_DEPTH, SCREEN_NEAR, format);
	if (!result)
		return false;

	m_DisplayPlane = new DisplayPlaneClass;
	result = m_DisplayPlane->Initialize(m_Direct3D->GetDevice(), displayWidth, displayHeight, m_RenderTexDisplay, m_Shader2D);
	if (!result)
		return false;
	m_DisplayPlane->SetPosition(0, 0, 5);

	m_RenderClass->AddDisplayPlane(m_DisplayPlane);

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

	if (m_ShaderMain)
	{
		m_ShaderMain->Shutdown();
		delete m_ShaderMain;
		m_ShaderMain = 0;
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

	if (m_Shader2D)
	{
		m_Shader2D->Shutdown();
		delete m_Shader2D;
		m_Shader2D = 0;
	}

	if (m_ShaderFont)
	{
		m_ShaderFont->Shutdown();
		delete m_ShaderFont;
		m_ShaderFont = 0;
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

	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
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

	if (m_ShaderMain)
	{
		m_ShaderMain->Shutdown();
		delete m_ShaderMain;
		m_ShaderMain = 0;
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

	m_Bitmap->Update(frameTime);

	static float rotation = 0.0f;
	rotation -= 0.0174532925f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	static float time = 0.0f;
	time += 0.1f;

	m_MadelineGO1->SetRotation(0, rotation * 10.0f, 0);
	m_IcosphereGO->SetRotation(rotation * 50.0f, 0, 0);
	//m_SpinnerObj->SetPosition(m_SpinnerObj->m_PosX + time, m_SpinnerObj->m_PosY);

	int mouseX, mouseY;
	bool mouseDown;
	if (Input->IsKeyPressed(DIK_ESCAPE))
		return false;
	Input->GetMouseLocation(mouseX, mouseY);
	mouseDown = Input->IsMousePressed();
	if (!UpdateMouseStrings(mouseX, mouseY, mouseDown))
		return false;
	
	m_MouseObj->SetPosition(mouseX - 635, -mouseY + 400);

	m_Camera->Frame(Input, frameTime);

	return Render() && UpdateFps();
}

bool ApplicationClass::Render()
{
	bool result;

	XMFLOAT4 diffuseColor[4], lightPosition[4];
	for (int i = 0; i < m_numLights; i++)
	{
		diffuseColor[i] = m_Lights[i].GetDiffuseColor();
		lightPosition[i] = m_Lights[i].GetPosition();
	}

	auto now = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_startTime).count();

	unordered_map<string, any> arguments =
	{
		{"CameraPos", m_Camera->GetPosition()},
		{"DiffuseColor", diffuseColor},
		{"LightPosition", lightPosition},
		{"DirLight", m_DirLight},
		{"Time", time},
		{"FogStart", 0.0f},
		{"FogEnd", 20.0f},
		{"ClipPlane", XMFLOAT4(0.0f, -1.0f, 0.0f, 2.5f)},
		{"Translation", XMFLOAT2(0.0f, 0.0f)},
		{"TranslationTimeMult", 0.0f},
		{"Alpha", 1.0f},
		{"WaterRRScale", 0.01f},
		{"Name", ""}
	};	

	result = m_RenderClass->Render(arguments);
	if (!result)
		return false;
	
	return true;
}

bool ApplicationClass::UpdateFps()
{
	char tempString[16], finalString[16];
	float red, green, blue;

	m_Fps->Frame();
	int fps = m_Fps->GetFps();

	if (m_previousFps == fps)
		return true;

	m_previousFps = fps;

	if (fps > 99999)
		fps = 99999;

	sprintf_s(tempString, "%d", fps);
	strcpy_s(finalString, "Fps: ");
	strcat_s(finalString, tempString);

	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	m_FpsString->SetColor(red, green, blue);
	m_FpsString->SetText(finalString);

	return m_FpsString->UpdateText();
}

bool ApplicationClass::UpdateMouseStrings(int mouseX, int mouseY, bool mouseDown)
{
	char tempString[16], finalString[32];
	bool result;

	// Convert the mouse X integer to string format.
	sprintf_s(tempString, "%d", mouseX);

	strcpy_s(finalString, "Mouse X: ");
	strcat_s(finalString, tempString);

	m_TextStringMouseX->SetText(finalString);
	result = m_TextStringMouseX->UpdateText();
	if (!result)
		return false;

	// Convert the mouse Y integer to string format.
	sprintf_s(tempString, "%d", mouseY);

	strcpy_s(finalString, "Mouse Y: ");
	strcat_s(finalString, tempString);

	m_TextStringMouseY->SetText(finalString);
	result = m_TextStringMouseY->UpdateText();
	if (!result)
		return false;

	// Setup the mouse button string.
	sprintf_s(finalString, sizeof(finalString), "Mouse Button: %s", (mouseDown ? "Yes" : "No"));

	m_TextStringMouseBttn->SetText(finalString);
	result = m_TextStringMouseBttn->UpdateText();
	if (!result)
		return false;

	return true;
}