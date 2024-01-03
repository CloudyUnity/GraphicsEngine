#include "RenderClass.h"

RenderClass::RenderClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Frustum = 0;
}

RenderClass::RenderClass(const RenderClass&)
{
}

RenderClass::~RenderClass()
{
}

bool RenderClass::Initialize(D3DClass* d3d, CameraClass* cam, FrustumClass* frustum)
{
	m_Direct3D = d3d;
	m_Camera = cam;
	m_Frustum = frustum;

	return true;
}

void RenderClass::Shutdown()
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

	for (auto go : m_AllTextClassList)
	{
		go->Shutdown();
		delete go;
	}
}

void RenderClass::AddGameObject(GameObjectClass* go)
{
	m_AllGameObjectList.push_back(go);
}

void RenderClass::AddGameObject2D(GameObjectClass2D* go)
{
	m_All2DGameObjectList.push_back(go);
}

void RenderClass::AddTextClass(TextClass* go)
{
	m_AllTextClassList.push_back(go);
}

void RenderClass::AddDisplayPlane(DisplayPlaneClass* go)
{
	m_AllDisplayPlaneList.push_back(go);
}

bool RenderClass::Render(unordered_map<string, any> arguments, RenderTextureClass* reflectionRendTex, float reflectHeight)
{
	XMMATRIX viewMatrix, displayProjMatrix, projectionMatrix, orthoMatrix;
	bool result;	

	m_Direct3D->BeginScene(FOG_COLOR_R, FOG_COLOR_G, FOG_COLOR_B, FOG_COLOR_A);

	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Frustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);

	result = RenderToReflectionTexture(reflectionRendTex, reflectHeight, &arguments);
	if (!result)
		return false;		

	result = RenderScene(viewMatrix, projectionMatrix, arguments);
	if (!result)
		return false;

	result = RenderDisplayPlanes(arguments);
	if (!result)
		return false;

	result = Render2D(arguments);
	if (!result)
		return false;

	m_Direct3D->EndScene();

	return true;
}

bool RenderClass::RenderScene(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, unordered_map<string, any> arguments, string skipGO)
{
	for (auto go : m_AllGameObjectList)
	{
		if (!m_Frustum->CheckSphere(go->m_PosX, go->m_PosY, go->m_PosZ, go->GetBoundingRadius()))
			continue;

		if (go->m_NameIdentifier == "Madeline2")
			arguments.at("TranslationTimeMult") = 0.2f;
		else
			arguments.at("TranslationTimeMult") = 0.0f;

		if (go->m_NameIdentifier == "IcosphereTrans")
			arguments.at("Alpha") = 0.1f;
		else
			arguments.at("Alpha") = 1.0f;

		if (go->m_NameIdentifier == skipGO)
			continue;

		bool result = go->Render(m_Direct3D->GetDeviceContext(), viewMatrix, projectionMatrix, arguments);
		if (!result)
			return false;
	}

	return true;
}

bool RenderClass::RenderToReflectionTexture(RenderTextureClass* rendTex, float height, unordered_map<string, any>* args)
{
	XMMATRIX reflectViewMatrix, projectionMatrix;

	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Camera->RenderReflection(height);		
	m_Camera->GetReflectionViewMatrix(reflectViewMatrix);
	args->insert({ "ReflectionMatrix", reflectViewMatrix });

	rendTex->SetRenderTarget(m_Direct3D->GetDeviceContext());
	rendTex->ClearRenderTarget(m_Direct3D->GetDeviceContext(), FOG_COLOR_R, FOG_COLOR_G, FOG_COLOR_B, FOG_COLOR_A);

	bool result = RenderScene(reflectViewMatrix, projectionMatrix, *args, "GlassCube");
	if (!result)
		return false;

	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();	

	return true;
}

bool RenderClass::RenderToTexture(RenderTextureClass* rendTex, unordered_map<string, any> args)
{
	XMMATRIX viewMatrix, displayProjMatrix;

	m_Camera->GetViewMatrix(viewMatrix);

	rendTex->SetRenderTarget(m_Direct3D->GetDeviceContext());
	rendTex->ClearRenderTarget(m_Direct3D->GetDeviceContext(), FOG_COLOR_R, FOG_COLOR_G, FOG_COLOR_B, FOG_COLOR_A);
	rendTex->GetProjectionMatrix(displayProjMatrix);

	bool result = RenderScene(viewMatrix, displayProjMatrix, args);
	if (!result)
		return false;

	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();	

	return true;
}

bool RenderClass::RenderDisplayPlanes(unordered_map<string, any> args)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	for (auto go : m_AllDisplayPlaneList)
	{
		RenderToTexture(go->m_RenderTexture, args);
		result = go->Render(m_Direct3D->GetDeviceContext(), viewMatrix, projectionMatrix, args);
		if (!result)
			return false;
	}

	return true;
}

bool RenderClass::Render2D(unordered_map<string, any> args)
{
	XMMATRIX viewMatrix2D, orthoMatrix;
	bool result;

	m_Direct3D->TurnZBufferOff();
	m_Camera->Get2DViewMatrix(viewMatrix2D);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	for (auto go : m_All2DGameObjectList)
	{
		result = go->Render(m_Direct3D->GetDeviceContext(), viewMatrix2D, orthoMatrix, args);
		if (!result)
			return false;
	}

	for (auto go : m_AllTextClassList)
	{
		result = go->Render(m_Direct3D->GetDeviceContext(), viewMatrix2D, orthoMatrix, args);
		if (!result)
			return false;
	}

	m_Direct3D->TurnZBufferOn();	

	return true;
}