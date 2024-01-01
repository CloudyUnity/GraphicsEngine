#include "ApplicationClass.h"
#include "ModelParser.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_MadelineModel = 0;
	m_TextureShader = 0;
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
	m_RenderTexture = 0;
	m_DisplayPlane = 0;
	m_mountainGO = 0;
	m_IcosphereGO = 0;
	m_transIcoGO = 0;
	m_MadelineGO1 = 0;
	m_MadelineGO2 = 0;
	m_IcosphereModel = 0;

	m_startTime = std::chrono::high_resolution_clock::now();

	//ModelParser::ParseFile("C:\\Users\\finnw\\OneDrive\\Documents\\3D objects\\MountFuji.obj");
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
	char modelFilename[128];
	char bitmapFilename[128];
	char vertexShader[128], fragShader[128];

	m_Direct3D = new D3DClass;

	bool result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
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

	strcpy_s(vertexShader, "../GraphicsEngine/Fog.vs");
	strcpy_s(fragShader, "../GraphicsEngine/Fog.ps");
	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(fragShader, "../GraphicsEngine/2D.ps");
	m_2DShader = new TextureShaderClass;
	result = m_2DShader->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(fragShader, "../GraphicsEngine/Font.ps");
	m_FontShader = new TextureShaderClass;
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(fragShader, "../GraphicsEngine/Display.ps");
	m_DisplayShader = new TextureShaderClass;
	result = m_DisplayShader->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, fragShader);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../GraphicsEngine/Models/Madeline.txt");
	char** textures = new char* [4];
	for (int i = 0; i < 4; ++i)
		textures[i] = new char[128];

	strcpy_s(textures[0], 128, "../GraphicsEngine/Data/Celeste.tga");
	strcpy_s(textures[1], 128, "../GraphicsEngine/Data/Moss.tga");
	strcpy_s(textures[2], 128, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	strcpy_s(textures[3], 128, "../GraphicsEngine/Data/MossNormal.tga");

	m_MadelineModel = new ModelClass;
	result = m_MadelineModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textures, 4);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}		
	
	m_MadelineGO1 = new GameObjectClass;
	m_MadelineGO1->Initialize(m_MadelineModel, m_TextureShader);	

	m_MadelineGO2 = new GameObjectClass;
	m_MadelineGO2->Initialize(m_MadelineModel, m_TextureShader);
	m_MadelineGO2->SetPosition(3, 0, 3);
	m_MadelineGO2->SetScale(0.5f, 0.5f, 0.5f);	

	textures = new char* [4];
	for (int i = 0; i < 4; ++i)
		textures[i] = new char[128];
	strcpy_s(modelFilename, "../GraphicsEngine/Models/Icosphere.txt");
	strcpy_s(textures[0], 128, "../GraphicsEngine/Data/Moss.tga");
	strcpy_s(textures[1], 128, "../GraphicsEngine/Data/Moss.tga");
	strcpy_s(textures[2], 128, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	strcpy_s(textures[3], 128, "../GraphicsEngine/Data/MossNormal.tga");

	m_IcosphereModel = new ModelClass;
	result = m_IcosphereModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textures, 4);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_IcosphereGO = new GameObjectClass;
	m_IcosphereGO->Initialize(m_IcosphereModel, m_TextureShader);
	m_IcosphereGO->SetScale(5, 5, 5);
	m_IcosphereGO->SetPosition(0, 0, 15);

	m_transIcoGO = new GameObjectClass;
	m_transIcoGO->Initialize(m_IcosphereModel, m_TextureShader);
	m_transIcoGO->SetPosition(3, 0.5f, 3);
	m_transIcoGO->SetScale(1.0f, 1.0f, 1.0f);

	textures = new char* [4];
	for (int i = 0; i < 4; ++i)
		textures[i] = new char[128];
	strcpy_s(modelFilename, "../GraphicsEngine/Models/MountFuji.txt");
	strcpy_s(textures[0], 128, "../GraphicsEngine/Data/Snow.tga");
	strcpy_s(textures[1], 128, "../GraphicsEngine/Data/Snow.tga");
	strcpy_s(textures[2], 128, "../GraphicsEngine/Data/DefaultAlphaMap.tga");
	strcpy_s(textures[3], 128, "../GraphicsEngine/Data/DefaultNormal.tga");

	ModelClass* mountFuji = new ModelClass;
	result = mountFuji->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textures, 4);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_mountainGO = new GameObjectClass;
	m_mountainGO->Initialize(mountFuji, m_TextureShader);
	m_mountainGO->SetScale(0.3f, 0.3f, 0.3f);
	m_mountainGO->SetPosition(1, -13, 15);

	m_AllGameObjectList.push_back(m_MadelineGO1);
	m_AllGameObjectList.push_back(m_MadelineGO2);
	m_AllGameObjectList.push_back(m_IcosphereGO);
	m_AllGameObjectList.push_back(m_transIcoGO);
	m_AllGameObjectList.push_back(m_mountainGO);

	strcpy_s(bitmapFilename, "../GraphicsEngine/Animations/Spinner.txt");
	m_Bitmap = new BitmapClass;
	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename);
	if (!result)
		return false;
	m_SpinnerObj = new GameObjectClass2D;
	m_SpinnerObj->Initialize(m_Bitmap, m_2DShader);
	m_SpinnerObj->SetPosition(1000, 50);
	m_SpinnerObj->SetScale(0.75f, 0.75f);
	m_All2DGameObjectList.push_back(m_SpinnerObj);

	strcpy_s(bitmapFilename, "../GraphicsEngine/Animations/MouseAnim.txt");
	m_MouseCursor = new BitmapClass;
	result = m_MouseCursor->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename);
	if (!result)
		return false;
	m_MouseObj = new GameObjectClass2D;
	m_MouseObj->Initialize(m_MouseCursor, m_2DShader);
	m_MouseObj->SetScale(0.1f, 0.1f);
	m_MouseObj->SetRotation(225);
	m_All2DGameObjectList.push_back(m_MouseObj);

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
	result = m_TextString1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_FontShader);
	if (!result)
		return false;
	m_TextString1->SetColor(1, 0, 0);
	m_TextString1->SetFont(m_Font);
	m_TextString1->SetPosition(10, 10);
	m_TextString1->SetText(text);
	m_TextString1->UpdateText();
	m_AllTextClassList.push_back(m_TextString1);

	strcpy_s(text, "Sample Text 2");
	m_TextString2 = new TextClass;
	result = m_TextString2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_FontShader);
	if (!result)
		return false;
	m_TextString2->SetColor(0, 1, 0);
	m_TextString2->SetFont(m_Font);
	m_TextString2->SetPosition(15, 30);
	m_TextString2->SetText(text);
	m_TextString2->UpdateText();
	m_AllTextClassList.push_back(m_TextString2);

	m_Fps = new FpsClass();
	m_Fps->Initialize();
	m_previousFps = -1;

	m_FpsString = new TextClass;
	result = m_FpsString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_FontShader);
	if (!result)
		return false;
	m_FpsString->SetFont(m_Font);
	m_FpsString->SetPosition(10, 60);
	m_AllTextClassList.push_back(m_FpsString);

	m_TextStringMouseX = new TextClass;
	m_TextStringMouseY = new TextClass;
	m_TextStringMouseBttn = new TextClass;
	result = m_TextStringMouseX->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_FontShader);
	if (!result)
		return false;
	result = m_TextStringMouseY->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_FontShader);
	if (!result)
		return false;
	result = m_TextStringMouseBttn->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_FontShader);
	if (!result)
		return false;	
	m_TextStringMouseX->SetFont(m_Font);
	m_TextStringMouseY->SetFont(m_Font);
	m_TextStringMouseBttn->SetFont(m_Font);
	m_TextStringMouseX->SetPosition(10, 80);
	m_TextStringMouseY->SetPosition(10, 100);
	m_TextStringMouseBttn->SetPosition(10, 120);
	m_AllTextClassList.push_back(m_TextStringMouseX);
	m_AllTextClassList.push_back(m_TextStringMouseY);
	m_AllTextClassList.push_back(m_TextStringMouseBttn);

	m_RenderTexture = new RenderTextureClass;
	int texHeight = 256;
	int texWidth = 256;
	int format = 1;
	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), texWidth, texHeight, SCREEN_DEPTH, SCREEN_NEAR, format);
	if (!result)
		return false;

	m_DisplayPlane = new DisplayPlaneClass;
	int displayWidth = 1;
	int displayHeight = 1;
	result = m_DisplayPlane->Initialize(m_Direct3D->GetDevice(), displayWidth, displayHeight);
	if (!result)
		return false;

	return true;
}

void ApplicationClass::Shutdown()
{
	for (auto go : m_AllGameObjectList) 
	{
		go->Shutdown();
		delete go;
	}

	for (auto go : m_All2DGameObjectList)
	{
		go->Shutdown();
		delete go;
	}

	if (m_MadelineModel) 
	{
		m_MadelineModel->Shutdown();
		delete m_MadelineModel;
		m_MadelineModel = 0;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_DisplayPlane)
	{
		m_DisplayPlane->Shutdown();
		delete m_DisplayPlane;
		m_DisplayPlane = 0;
	}

	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_2DShader)
	{
		m_2DShader->Shutdown();
		delete m_2DShader;
		m_2DShader = 0;
	}

	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	if (m_TextStringMouseX)
	{
		m_TextStringMouseX->Shutdown();
		delete m_TextStringMouseX;
		m_TextStringMouseX = 0;
	}
	if (m_TextStringMouseY)
	{
		m_TextStringMouseY->Shutdown();
		delete m_TextStringMouseY;
		m_TextStringMouseY = 0;
	}
	if (m_TextStringMouseBttn)
	{
		m_TextStringMouseBttn->Shutdown();
		delete m_TextStringMouseBttn;
		m_TextStringMouseBttn = 0;
	}

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if (m_FpsString)
	{
		m_FpsString->Shutdown();
		delete m_FpsString;
		m_FpsString = 0;
	}

	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	if (m_TextString2)
	{
		m_TextString2->Shutdown();
		delete m_TextString2;
		m_TextString2 = 0;
	}

	if (m_TextString1)
	{
		m_TextString1->Shutdown();
		delete m_TextString1;
		m_TextString1 = 0;
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

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
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
	XMMATRIX viewMatrix, displayProjMatrix, projectionMatrix, orthoMatrix;
	bool result;	

	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);	

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
		{"Alpha", 1.0f}
	};	

	float fogColor = 0.5f;
	m_Direct3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);
	m_Frustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);

	for (auto go : m_AllGameObjectList) 
	{
		if (!m_Frustum->CheckSphere(go->m_PosX, go->m_PosY, go->m_PosZ, go->GetBoundingRadius()))
			continue;

		if (go == m_MadelineGO2)
			arguments.at("TranslationTimeMult") = 0.2f;
		else
			arguments.at("TranslationTimeMult") = 0.0f;

		if (go == m_transIcoGO)
			arguments.at("Alpha") = 0.1f;
		else
			arguments.at("Alpha") = 1.0f;

		result = go->Render(m_Direct3D->GetDeviceContext(), viewMatrix, projectionMatrix, arguments);
		if (!result)
			return false;
	}		

	bool renderToTexture = true;
	if (renderToTexture)
	{
		XMMATRIX worldMatrix = XMMatrixTranslation(0, 0, 5);		

		m_DisplayPlane->Render(m_Direct3D->GetDeviceContext());
		result = m_DisplayShader->Render(m_Direct3D->GetDeviceContext(), m_DisplayPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_RenderTexture->GetShaderResourceView());
		if (!result)
			return false;
		
		m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());		
		m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), fogColor, fogColor, fogColor, 1.0f);
		m_RenderTexture->GetProjectionMatrix(displayProjMatrix);

		for (auto go : m_AllGameObjectList)
		{
			if (!m_Frustum->CheckSphere(go->m_PosX, go->m_PosY, go->m_PosZ, go->GetBoundingRadius()))
				continue;

			result = go->Render(m_Direct3D->GetDeviceContext(), viewMatrix, displayProjMatrix, arguments);
			if (!result)
				return false;
		}

		m_Direct3D->SetBackBufferRenderTarget();
		m_Direct3D->ResetViewport();
	}
	
	m_Direct3D->TurnZBufferOff();
	m_Camera->Get2DViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	for (auto go : m_All2DGameObjectList)
	{
		result = go->Render(m_Direct3D->GetDeviceContext(), viewMatrix, orthoMatrix, arguments);
		if (!result)
			return false;
	}

	for (auto go : m_AllTextClassList)
	{
		result = go->Render(m_Direct3D->GetDeviceContext(), viewMatrix, orthoMatrix, arguments);
		if (!result)
			return false;
	}

	m_Direct3D->TurnZBufferOn();

	m_Direct3D->EndScene();

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