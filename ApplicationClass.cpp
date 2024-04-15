#include "ApplicationClass.h"
#include "ModelParser.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Timer = 0;
	m_Font = 0;
	m_Fps = 0;
	m_RenderClass = 0;
	m_Frustum = 0;
	m_Parameters = 0;

	m_FpsString = 0;
	m_TextStringMouseBttn = 0;
	m_TextStringMouseX = 0;
	m_TextStringMouseY = 0;

	m_currentScene = 0;

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

	m_Timer = new TimerClass;
	result = m_Timer->Initialize();
	if (!result)
		return false;

	m_Frustum = new FrustumClass;

	m_Fps = new FpsClass();
	m_Fps->Initialize();

	ShaderClass* shader2D = 0;
	result = InitializeShader(hwnd, &shader2D, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/2D.ps");
	if (!result)
		return false;

	m_RenderClass = new RenderClass;
	result = m_RenderClass->Initialize(m_Direct3D, m_Frustum, m_Settings, shader2D);
	if (!result)
		return false;		

	m_startTime = std::chrono::high_resolution_clock::now();

	m_Parameters = new ShaderClass::ShaderParamsGlobalType;
	UpdateParameters();

	// SHADERS

	ShaderClass* shaderDepth = 0, * shaderFont = 0;
	result = 
		InitializeShader(hwnd, &shaderFont, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Font.ps") &&
		InitializeShader(hwnd, &shaderDepth, "../GraphicsEngine/Depth.vs", "../GraphicsEngine/Depth.ps");
	if (!result)
		return false;

	m_RenderClass->SetDepthShader(shaderDepth);

	// TEXT

	m_Font = new FontClass;
	result = m_Font->Initialize(device, deviceContext, 0);
	if (!result)
		return false;

	int maxLength = 32;
	result =
		InitializeTextClass(&m_FpsString, shaderFont, m_Font, maxLength) &&
		InitializeTextClass(&m_TextStringMouseX, shaderFont, m_Font, maxLength) &&
		InitializeTextClass(&m_TextStringMouseY, shaderFont, m_Font, maxLength) &&
		InitializeTextClass(&m_TextStringMouseBttn, shaderFont, m_Font, maxLength);
	if (!result)
		return false;

	m_FpsString->SetPosition(10, 60);

	m_TextStringMouseX->SetPosition(10, 80);
	m_TextStringMouseX->m_shaderUniformData.pixel.pixelColor = XMFLOAT4(1, 1, 1, 1);
	m_TextStringMouseY->SetPosition(10, 100);
	m_TextStringMouseY->m_shaderUniformData.pixel.pixelColor = XMFLOAT4(1, 1, 1, 1);
	m_TextStringMouseBttn->SetPosition(10, 120);
	m_TextStringMouseBttn->m_shaderUniformData.pixel.pixelColor = XMFLOAT4(1, 1, 1, 1);

	// SCENES

	SceneTestClass* sceneTest = new SceneTestClass();
	sceneTest->InitializeMembers(m_Settings, m_Direct3D, m_RenderClass);
	m_sceneList.push_back(sceneTest);

	SceneOceanClass* sceneOcean = new SceneOceanClass();
	sceneOcean->InitializeMembers(m_Settings, m_Direct3D, m_RenderClass);
	m_sceneList.push_back(sceneOcean);

	m_currentScene = sceneOcean;
	m_currentScene->InitializeScene(hwnd);
	m_currentScene->OnSwitchTo();
	m_currentScene->m_InitializedScene = true;
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

	m_loadedAssetsList.push_back(*ptr);

	return true;
}

bool ApplicationClass::InitializeTextClass(TextClass** ptr, ShaderClass* shader, FontClass* font, int maxLength)
{
	*ptr = new TextClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), SCREEN_X, SCREEN_Y, maxLength, shader);
	if (!result)
		return false;

	(*ptr)->SetFont(font);

	m_overlayTextList.push_back(*ptr);
	m_loadedAssetsList.push_back(*ptr);

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

	m_Parameters->reflectionEnabled = m_Settings->m_CurrentData.ReflectionEnabled;

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
	
	float a = 0.4f;
	float p = 0.8f;
	float w = 1.0f;
	float theta = 104;
	for (int i = 0; i < SIN_COUNT; i++)
	{
		m_Parameters->oceanSine.ampPhaseFreq[i] = XMFLOAT4(a, p, w, theta);
		a *= 0.8f;
		p *= -1.1f;
		w *= -1.12f;
		theta += 30.0f;
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

	for (auto asset : m_loadedAssetsList)
	{
		asset->Shutdown();
		delete asset;
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

bool ApplicationClass::Frame(HWND hwnd, InputClass* input)
{
	int outSettingsIndex;
	if (input->IsNumberPressed(outSettingsIndex))
	{
		m_Settings->ChangeSettings(outSettingsIndex);
		UpdateParameters();
	}

	m_Timer->Frame();
	float frameTime = m_Timer->GetTime();
	
	if (input->GetKeyDown(DIK_ESCAPE))
		return false;

	if (input->GetKeyDown(DIK_SPACE))
		SwitchScene(hwnd);

	int mouseX, mouseY;
	input->GetMouseLocation(mouseX, mouseY);

	bool mouseDown = input->IsMousePressed();
	if (!UpdateMouseStrings(mouseX, mouseY, mouseDown))
		return false;	

	m_Fps->Frame();

	m_currentScene->ParticlesFrame(frameTime);
	m_currentScene->Frame(input, frameTime);

	return Render() && m_Fps->UpdateFPS(m_FpsString) && LateFrame(input, frameTime);
}

bool ApplicationClass::LateFrame(InputClass* input, float frameTime)
{
	return m_currentScene->LateFrame(input, frameTime);
}

bool ApplicationClass::SwitchScene(HWND hwnd)
{
	for (int i = 0; i < m_sceneList.size(); i++)
	{
		if (m_sceneList.at(i) != m_currentScene)
			continue;

		int newIndex = (i + 1) % m_sceneList.size();
		m_currentScene = m_sceneList.at(newIndex);		
		if (!m_currentScene->m_InitializedScene)
		{
			m_currentScene->InitializeScene(hwnd);			
			m_currentScene->m_InitializedScene = true;
		}
		m_currentScene->OnSwitchTo();
		return true;
	}

	return false;
}

bool ApplicationClass::Render()
{
	bool result;

	auto now = std::chrono::high_resolution_clock::now();
	m_Parameters->utils.time = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_startTime).count();

	m_currentScene->SetParameters(m_Parameters);

	SceneClass::SceneDataType sceneData; // Sus code
	sceneData = *(m_currentScene->GetSceneData());
	for (auto overlayText : m_overlayTextList)
		sceneData.TextList.push_back(overlayText);
	
	result = m_RenderClass->Render(m_Settings, m_Parameters, &sceneData);
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