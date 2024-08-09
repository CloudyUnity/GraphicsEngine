#include "SceneClass.h"

SceneClass::SceneClass(string name)
{
	m_Name = name;

	m_sceneData = new SceneDataType();
	m_InitializedScene = false;
}

bool SceneClass::InitializeMembers(Settings* settings, D3DClass* d3d, RenderClass* renderClass)
{
	m_settings = settings;
	m_Direct3D = d3d;
	m_RenderClass = renderClass;
	return true;
}

bool SceneClass::InitializeScene(HWND hwnd)
{
	return false;
}

bool SceneClass::ParticlesFrame(float frameTime)
{
	for (auto ps : m_renderedPSList)
	{
		ps->m_disabled = !m_settings->m_CurrentData.ParticlesEnabled;

		bool result = ps->Frame(m_Direct3D->GetDeviceContext(), frameTime);
		if (!result)
			return false;
	}

	return true;
}

bool SceneClass::Frame(InputClass*, float frameTime)
{
	return true;
}

bool SceneClass::LateFrame(InputClass* input, float frameTime)
{
	return true;
}

void SceneClass::SetParameters(ShaderClass::ShaderParamsGlobalType*)
{
}

void SceneClass::OnSwitchTo()
{
}

void SceneClass::Shutdown()
{
	if (m_sceneData)
	{
		delete m_sceneData;
	}

	for (auto asset : m_loadedAssetsList)
	{
		asset->Shutdown();
		delete asset;
	}
}

SceneClass::SceneDataType* SceneClass::GetSceneData()
{
	return m_sceneData;
}

bool SceneClass::CreateModel(HWND hwnd, ModelClass** ptr, const char* name)
{
	LogClass::LogStart(string("Initialising Model: ") + name);

	char modelFilename[128];
	strcpy_s(modelFilename, name);

	*ptr = new ModelClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_loadedAssetsList.push_back(*ptr);

	LogClass::LogEnd(string("Model Initialised: ") + name);

	return true;
}

bool SceneClass::CreateModel(HWND hwnd, ModelLineListClass** ptr, vector<XMFLOAT3> points)
{
	LogClass::LogStart(string("Initialising Model (LineList): "));

	*ptr = new ModelLineListClass();
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), points);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_loadedAssetsList.push_back(*ptr);

	LogClass::LogEnd(string("Model (LineList) Initialised: "));

	return true;
}

bool SceneClass::CreateShader(HWND hwnd, ShaderClass** ptr, const char* vertexName, const char* fragName, bool clamp)
{
	LogClass::LogStart(string("Initialising Shader: vs:") + vertexName + string(" ps:") + fragName);

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

	LogClass::LogEnd(string("Shader Initialised: vs:") + vertexName + string(" ps:") + fragName);

	return true;
}

bool SceneClass::CreateShader(HWND hwnd, ShaderTessClass** ptr, const char* vertexName, const char* hullName, const char* domainName, const char* fragName, bool clampSamplerMode)
{
	LogClass::LogStart(string("Initialising Shader (Tesselation): vs:") + vertexName + string(" hs:") + hullName + string(" ds:") + domainName + string(" ps:") + fragName);

	char vertexShader[128], hullShader[128], domainShader[128], fragShader[128];

	strcpy_s(vertexShader, vertexName);
	strcpy_s(hullShader, hullName);
	strcpy_s(domainShader, domainName);
	strcpy_s(fragShader, fragName);

	*ptr = new ShaderTessClass();
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), hwnd, vertexShader, hullShader, domainShader, fragShader, clampSamplerMode);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_loadedAssetsList.push_back(*ptr);

	LogClass::LogEnd(string("Shader (Tesselation) Initialised: vs:") + vertexName + string(" hs:") + hullName + string(" ds:") + domainName + string(" ps:") + fragName);

	return true;
}

void SceneClass::CreateGameObject(ModelClass* model, ShaderClass* shader, TextureSetClass* texSet, bool transparent, const char* name, GameObjectClass*& ptr)
{
	LogClass::LogStart(string("Initialising GameObject: ") + name);

	ptr = new GameObjectClass();

	ptr->Initialize(model, shader, texSet, name);

	m_loadedAssetsList.push_back(ptr);

	if (transparent)
		m_sceneData->GoTransList.push_back(ptr);
	else
		m_sceneData->GoOpaqueList.push_back(ptr);

	LogClass::LogEnd(string("GameObject Initialised: ") + name);
}

void SceneClass::CreateGameObject(ModelClass* model, ShaderClass* shader, TextureSetClass* texSet, bool transparent, const char* name)
{
	LogClass::LogStart(string("Initialising GameObject: ") + name);

	GameObjectClass* ptr = new GameObjectClass();

	ptr->Initialize(model, shader, texSet, name);

	m_loadedAssetsList.push_back(ptr);

	if (transparent)
		m_sceneData->GoTransList.push_back(ptr);
	else
		m_sceneData->GoOpaqueList.push_back(ptr);

	LogClass::LogEnd(string("GameObject Initialised: ") + name);
}

void SceneClass::CreateGameObject2D(BitmapClass* bitmap, ShaderClass* shader, GameObjectClass2D** ptr)
{
	LogClass::LogStart(string("Initialising GameObject (2D)"));

	*ptr = new GameObjectClass2D;
	(*ptr)->Initialize(bitmap, shader);

	m_loadedAssetsList.push_back(*ptr);

	m_sceneData->Go2DList.push_back(*ptr);

	LogClass::LogEnd(string("GameObject(2D) Initialised"));
}

bool SceneClass::CreateText(TextClass** ptr, ShaderClass* shader, FontClass* font, int maxLength)
{
	LogClass::LogStart(string("Initialising Text"));

	*ptr = new TextClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), SCREEN_X, SCREEN_Y, maxLength, shader);
	if (!result)
		return false;

	(*ptr)->SetFont(font);

	m_loadedAssetsList.push_back(*ptr);

	m_sceneData->TextList.push_back(*ptr);

	LogClass::LogEnd(string("Text Initialised"));

	return true;
}

void SceneClass::CreateTexSet(TextureSetClass** ptr)
{
	LogClass::LogStart(string("Initialising TexSet"));

	*ptr = new TextureSetClass;

	m_loadedAssetsList.push_back(*ptr);

	LogClass::LogEnd(string("TexSet Initialised"));
}

bool SceneClass::CreateBitmap(BitmapClass** ptr, const char* filename)
{
	LogClass::LogStart(string("Initialising Bitmap: ") + filename);

	char bitmapFilename[128];

	strcpy_s(bitmapFilename, filename);
	*ptr = new BitmapClass;
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), SCREEN_X, SCREEN_Y, bitmapFilename);
	if (!result)
		return false;

	m_loadedAssetsList.push_back(*ptr);

	LogClass::LogEnd(string("Bitmap Initialised: ") + filename);

	return true;
}

bool SceneClass::CreateParticleSystem(ParticleSystemClass** ptr, ParticleSystemClass::ParticleSystemData data, ShaderClass* shader, TextureSetClass* texSet, const char* name)
{
	LogClass::LogStart(string("Initialising Particle System: ") + name);

	*ptr = new ParticleSystemClass();
	bool result = (*ptr)->Initialize(m_Direct3D->GetDevice(), data, shader, texSet, name);
	if (!result)
		return false;

	m_loadedAssetsList.push_back(*ptr);
	m_renderedPSList.push_back(*ptr);
	
	m_sceneData->PsList.push_back(*ptr);

	LogClass::LogEnd(string("Particle System Initialised: ") + name);

	return true;
}

bool SceneClass::CreateRenderTexture(RenderTextureClass** outRendTexPtr, ID3D11Device* device, int width, int height, float screenDepth, float screenNear, int format)
{
	LogClass::LogStart(string("Initialising Render Texture"));

	(*outRendTexPtr) = new RenderTextureClass();
	bool result = (*outRendTexPtr)->Initialize(device, width, height, screenDepth, screenNear, format);
	if (!result)
		return false;

	m_loadedAssetsList.push_back(*outRendTexPtr);

	LogClass::LogEnd(string("Render Texture Initialised"));

	return true;
}

bool SceneClass::CreateDisplayPlane(DisplayPlaneClass** outDisplayPtr, ID3D11Device* device, float width, float height, RenderTextureClass* rendTex, ShaderClass* shader, const char* name, bool postProcess, CameraClass* cam)
{
	LogClass::LogStart(string("Initialising Display Plane: ") + name);

	*outDisplayPtr = new DisplayPlaneClass();
	bool result = (*outDisplayPtr)->Initialize(device, width, height, rendTex, shader, name, cam);
	if (!result)
		return false;

	m_loadedAssetsList.push_back(*outDisplayPtr);
	if (postProcess)
		m_sceneData->PostProcessingLayers.push_back(*outDisplayPtr);
	else
		m_sceneData->DisplayPlaneList.push_back(*outDisplayPtr);

	LogClass::LogEnd(string("Display Plane Initialised: ") + name);

	return true;
}

void SceneClass::SubscribeToReflection(GameObjectClass* goPtr, int texSetIndex, int format)
{
	goPtr->SubscribeToReflection(m_Direct3D->GetDevice(), texSetIndex, format);
	m_sceneData->GoReflectList.push_back(goPtr);
}

void SceneClass::SubscribeToRefraction(GameObjectClass* goPtr, int texSetIndex, int format)
{
	goPtr->SubscribeToRefraction(m_Direct3D->GetDevice(), texSetIndex, format);
	m_sceneData->GoRefractList.push_back(goPtr);
}

void SceneClass::SubscribeToShadow(GameObjectClass* goPtr, int texSetIndex)
{
	goPtr->SubscribeToShadows(texSetIndex);
	m_sceneData->GoShadowList.push_back(goPtr);
}