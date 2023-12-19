#include "ApplicationClass.h"
#include "ModelParser.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
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
	m_MouseStrings = 0;

	//ModelParser::ParseFile("C:\\Users\\finnw\\OneDrive\\Documents\\3D objects\\Mountain.obj");
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char testString1[32], testString2[32], fpsString[32];
	char textureFilename1[128], textureFilename2[128], alphaMapFilename[128], normalFilename[128];
	char modelFilename[128];
	char bitmapFilename[128];
	char mouseString1[32], mouseString2[32], mouseString3[32];

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

	strcpy_s(modelFilename, "../GraphicsEngine/Models/Madeline.txt");
	strcpy_s(textureFilename1, "../GraphicsEngine/Data/Celeste.tga");
	strcpy_s(textureFilename2, "../GraphicsEngine/Data/Moss.tga");
	strcpy_s(alphaMapFilename, "../GraphicsEngine/Data/AlphaMap.tga");
	strcpy_s(normalFilename, "../GraphicsEngine/Data/MossNormal.tga");
	m_Model = new ModelClass;
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2, nullptr, normalFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}	

	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(bitmapFilename, "../GraphicsEngine/Animations/Spinner.txt");
	m_Bitmap = new BitmapClass;
	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename, 0, 0);
	if (!result)
		return false;

	strcpy_s(bitmapFilename, "../GraphicsEngine/Animations/MouseAnim.txt");
	m_MouseCursor = new BitmapClass;
	result = m_MouseCursor->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, bitmapFilename, 0, 0);
	if (!result)
		return false;

	m_Timer = new TimerClass;
	result = m_Timer->Initialize();
	if (!result)
		return false;

	m_numLights = 4;
	m_Lights = new LightClass[m_numLights];

	m_Lights[0].SetDiffuseColor(1.0f, 0.5f, 0.5f, 0.5f);
	m_Lights[0].SetPosition(100.0f, 0.0f, 0.0f);

	m_Lights[1].SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_Lights[1].SetPosition(100.0f, 0.0f, 0.0f);

	m_Lights[2].SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_Lights[2].SetPosition(100.0f, 0.0f, 0.0f);

	m_Lights[3].SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_Lights[3].SetPosition(100.0f, 0.0f, 0.0f);

	m_DirLight = new LightClass();
	m_DirLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_DirLight->SetDiffuseColor(0.5f, 1.0f, 0.5f, 1.0f);
	m_DirLight->SetDirection(-1.0f, 0.0f, -1.0f);
	m_DirLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLight->SetSpecularPower(32.0f);

	m_Font = new FontClass;
	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if (!result)
		return false;
	
	strcpy_s(testString1, "The Badeline Digital Auction!");
	m_TextString1 = new TextClass;
	int maxLength = 32;
	int posX = 10;
	int posY = 10;
	int r = 0;
	int g = 1;
	int b = 0;
	result = m_TextString1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_Font, testString1, posX, posY, r, g, b);
	if (!result)
		return false;

	strcpy_s(testString2, "Name your price!");
	m_TextString2 = new TextClass;
	maxLength = 32;
	posX = 15;
	posY = 30;
	r = 1;
	g = 0;
	b = 0;
	result = m_TextString2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_Font, testString2, posX, posY, r, g, b);
	if (!result)
		return false;

	m_Fps = new FpsClass();
	m_Fps->Initialize();
	m_previousFps = -1;
	strcpy_s(fpsString, "FPS: err");
	m_FpsString = new TextClass;
	maxLength = 32;
	posX = 10;
	posY = 60;
	r = 1;
	g = 1;
	b = 1;
	result = m_FpsString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, maxLength, m_Font, fpsString, posX, posY, r, g, b);
	if (!result)
		return false;

	strcpy_s(mouseString1, "Mouse X: 0");
	strcpy_s(mouseString2, "Mouse Y: 0");
	strcpy_s(mouseString3, "Mouse Button: No");
	m_MouseStrings = new TextClass[3];
	result = m_MouseStrings[0].Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, mouseString1, 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;
	result = m_MouseStrings[1].Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, mouseString2, 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;
	result = m_MouseStrings[2].Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, mouseString3, 10, 10, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if (m_MouseStrings)
	{
		m_MouseStrings[0].Shutdown();
		m_MouseStrings[1].Shutdown();
		m_MouseStrings[2].Shutdown();

		delete[] m_MouseStrings;
		m_MouseStrings = 0;
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

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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
	rotation -= 0.0174532925f * 0.8f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	int mouseX, mouseY;
	bool mouseDown;
	if (Input->IsKeyPressed(DIK_ESCAPE))
		return false;
	Input->GetMouseLocation(mouseX, mouseY);
	mouseDown = Input->IsMousePressed();
	if (!UpdateMouseStrings(mouseX, mouseY, mouseDown))
		return false;

	m_Camera->Frame(Input, frameTime);

	return Render(rotation, mouseX, mouseY) && UpdateFps();
}

bool ApplicationClass::Render(float rotation, int mousePosX, int mousePosY)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix, scaleMatrix, srMatrix, orthoMatrix;
	bool result;
	
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	XMFLOAT4 diffuseColor[4], lightPosition[4];
	for (int i = 0; i < m_numLights; i++)
	{
		diffuseColor[i] = m_Lights[i].GetDiffuseColor();
		lightPosition[i] = m_Lights[i].GetPosition();
	}

	rotateMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(0.0f, -1.0f, -2.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0),
		diffuseColor, lightPosition, m_DirLight->GetDirection(), m_DirLight->GetAmbientColor(), m_DirLight->GetDiffuseColor(), m_Camera->GetPosition(), m_DirLight->GetSpecularColor(),
		m_DirLight->GetSpecularPower(), m_Model->GetTexture(1), m_Model->GetTexture(2), m_Model->GetTexture(3));
	if (!result)
		return false;

	scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	translateMatrix = XMMatrixTranslation(2.0f, 0.0f, 0.0f);
	rotateMatrix = XMMatrixRotationY(rotation * 0.5);
	srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
	worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0),
		diffuseColor, lightPosition, m_DirLight->GetDirection(), m_DirLight->GetAmbientColor(), m_DirLight->GetDiffuseColor(), m_Camera->GetPosition(), m_DirLight->GetSpecularColor(),
		m_DirLight->GetSpecularPower());
	if (!result)
		return false;

	bool render2D = true;
	if (render2D) {
		m_Direct3D->EnableAlphaBlending();
		m_Direct3D->TurnZBufferOff();
		m_Camera->Get2DViewMatrix(viewMatrix);

		scaleMatrix = XMMatrixScaling(0.25f, 0.25f, 1.0f);
		translateMatrix = XMMatrixTranslation(-200.0f, 170.0f, 0.0f);
		worldMatrix = XMMatrixMultiply(scaleMatrix, translateMatrix);

		m_Bitmap->Render(m_Direct3D->GetDeviceContext());
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture(),
			nullptr, nullptr, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), m_Camera->GetPosition(), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 0);
		if (!result)
			return false;

		scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);		
		worldMatrix = scaleMatrix;

		m_TextString1->Render(m_Direct3D->GetDeviceContext());
		result = m_TextureShader->RenderFont(m_Direct3D->GetDeviceContext(), m_TextString1->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
			m_Font->GetTexture(), m_TextString1->GetPixelColor());
		if (!result)
			return false;

		m_TextString2->Render(m_Direct3D->GetDeviceContext());
		result = m_TextureShader->RenderFont(m_Direct3D->GetDeviceContext(), m_TextString2->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
			m_Font->GetTexture(), m_TextString2->GetPixelColor());
		if (!result)
			return false;

		m_FpsString->Render(m_Direct3D->GetDeviceContext());
		result = m_TextureShader->RenderFont(m_Direct3D->GetDeviceContext(), m_FpsString->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
			m_Font->GetTexture(), m_FpsString->GetPixelColor());
		if (!result)
			return false;

		for (int i = 0; i < 3; i++)
		{
			m_MouseStrings[i].Render(m_Direct3D->GetDeviceContext());

			result = m_TextureShader->RenderFont(m_Direct3D->GetDeviceContext(), m_MouseStrings[i].GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
				m_Font->GetTexture(), m_MouseStrings[i].GetPixelColor());
			if (!result)
				return false;
		}

		scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 1.0f);
		rotateMatrix = XMMatrixRotationZ(180);
		translateMatrix = XMMatrixTranslation(mousePosX - 635, -mousePosY + 400, 0.0f);		
		srMatrix = XMMatrixMultiply(rotateMatrix, scaleMatrix);
		worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

		m_MouseCursor->Render(m_Direct3D->GetDeviceContext());
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_MouseCursor->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_MouseCursor->GetTexture(),
			nullptr, nullptr, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), m_Camera->GetPosition(), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 0);
		if (!result)
			return false;

		m_Direct3D->TurnZBufferOn();
		m_Direct3D->DisableAlphaBlending();
	}

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

	return m_FpsString->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 60, red, green, blue);
}

bool ApplicationClass::UpdateMouseStrings(int mouseX, int mouseY, bool mouseDown)
{
	char tempString[16], finalString[32];
	bool result;

	// Convert the mouse X integer to string format.
	sprintf_s(tempString, "%d", mouseX);

	strcpy_s(finalString, "Mouse X: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = m_MouseStrings[0].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 80, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	// Convert the mouse Y integer to string format.
	sprintf_s(tempString, "%d", mouseY);

	strcpy_s(finalString, "Mouse Y: ");
	strcat_s(finalString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = m_MouseStrings[1].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 100, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	// Setup the mouse button string.
	sprintf_s(finalString, sizeof(finalString), "Mouse Button: %s", (mouseDown ? "Yes" : "No"));

	// Update the sentence vertex buffer with the new string information.
	result = m_MouseStrings[2].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 120, 1.0f, 1.0f, 1.0f);
	if (!result)
		return false;

	return true;
}