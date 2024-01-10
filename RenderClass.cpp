#include "RenderClass.h"

RenderClass::RenderClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Frustum = 0;

	m_framesSinceReflectionRender = 0;
	m_renderReflectionImmediately = false;
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

void RenderClass::SubscribeToReflection(ID3D11Device* device, GameObjectClass* go, int texSetNum, int format)
{
	m_ReflectionList.push_back(go);

	go->SubscribeToReflection(device, texSetNum, format);
}

void RenderClass::SubscribeToRefraction(ID3D11Device* device, GameObjectClass* go, int texSetNum, int format)
{
	m_RefractionList.push_back(go);

	go->SubscribeToRefraction(device, texSetNum, format);
}

void RenderClass::RenderReflectionNextAvailableFrame()
{
	m_renderReflectionImmediately = true;
}

bool RenderClass::Render(ShaderClass::ShaderParameters* params)
{
	XMMATRIX viewMatrix, displayProjMatrix, projectionMatrix, reflectViewMatrix;
	bool result;	

	m_Direct3D->BeginScene(FOG_COLOR_R, FOG_COLOR_G, FOG_COLOR_B, FOG_COLOR_A);

	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Frustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);

	m_framesSinceReflectionRender++;

	bool renderReflection = m_framesSinceReflectionRender >= REFLECTION_FRAME_DELAY || m_renderReflectionImmediately;
	if (REFLECTION_ENABLED && renderReflection)
	{
		// Set reflection matrices
		for (auto go : m_ReflectionList)
		{
			m_Camera->RenderReflection(go->m_PosY + go->m_ScaleY);
			m_Camera->GetReflectionViewMatrix(reflectViewMatrix);
			go->SetReflectionMatrix(reflectViewMatrix);
		}

		// Refraction comes first
		for (auto go : m_RefractionList)
		{
			result = RenderToRefractionTexture(go, params);
			if (!result)
				return false;
			go->SetRefractionTex();
		}

		// Then reflection
		for (auto go : m_ReflectionList)
		{
			result = RenderToReflectionTexture(go, params);
			if (!result)
				return false;
			go->SetReflectionTex();
		}

		m_framesSinceReflectionRender = 0;
		m_renderReflectionImmediately = false;
	}

	result = RenderScene(viewMatrix, projectionMatrix, params);
	if (!result)
		return false;

	result = RenderDisplayPlanes(params);
	if (!result)
		return false;

	result = Render2D(params);
	if (!result)
		return false;

	m_Direct3D->EndScene();

	return true;
}

bool RenderClass::RenderScene(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ShaderClass::ShaderParameters* params, string skipGO)
{
	params->matrix.view = viewMatrix;
	params->matrix.projection = projectionMatrix;

	auto savedParams = *params;

	for (auto go : m_AllGameObjectList)
	{
		if (go->m_NameIdentifier == skipGO)
			continue;

		if (!m_Frustum->CheckSphere(go->m_PosX, go->m_PosY, go->m_PosZ, go->GetBoundingRadius()))
			continue;

		if (go->m_NameIdentifier == "Madeline2")
			params->textureTranslation.timeMultiplier = 0.1f;
		else if (go->m_NameIdentifier == "Water")
			params->textureTranslation.timeMultiplier = (0.1f / go->m_ScaleX) / go->m_ScaleZ;

		if (go->m_NameIdentifier == "IcosphereTrans")
			params->alpha.alphaBlend = 0.1f;

		if (go->m_NameIdentifier == "IcosphereBig")
			params->clip.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 2.5f);

		params->reflection.reflectionMatrix = go->GetReflectionMatrix();

		bool result = go->Render(m_Direct3D->GetDeviceContext(), params);
		if (!result)
			return false;

		*params = savedParams;
	}

	return true;
}

bool RenderClass::RenderToReflectionTexture(GameObjectClass* go, ShaderClass::ShaderParameters* params)
{
	XMMATRIX reflectViewMatrix, projectionMatrix;

	float height = go->m_PosY + go->m_ScaleY;

	auto clipPlane = params->clip.clipPlane;

	params->clip.clipPlane = XMFLOAT4(0.0f, 1.0f, 0.0f, -height + 0.05f);

	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Camera->RenderReflection(height);		
	m_Camera->GetReflectionViewMatrix(reflectViewMatrix);	

	go->m_RendTexReflection->SetRenderTarget(m_Direct3D->GetDeviceContext());
	go->m_RendTexReflection->ClearRenderTarget(m_Direct3D->GetDeviceContext(), FOG_COLOR_R, FOG_COLOR_G, FOG_COLOR_B, FOG_COLOR_A);

	bool result = RenderScene(reflectViewMatrix, projectionMatrix, params, go->m_NameIdentifier);
	if (!result)
		return false;

	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();	

	params->clip.clipPlane = clipPlane;

	return true;
}

bool RenderClass::RenderToRefractionTexture(GameObjectClass* go, ShaderClass::ShaderParameters* params)
{
	XMMATRIX viewMatrix, projectionMatrix;

	float height = go->m_PosY + go->m_ScaleY;

	auto clipPlane = params->clip.clipPlane;

	params->clip.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, height + 0.1f);

	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);

	go->m_RendTexRefraction->SetRenderTarget(m_Direct3D->GetDeviceContext());
	go->m_RendTexRefraction->ClearRenderTarget(m_Direct3D->GetDeviceContext(), FOG_COLOR_R, FOG_COLOR_G, FOG_COLOR_B, FOG_COLOR_A);

	bool result = RenderScene(viewMatrix, projectionMatrix, params, go->m_NameIdentifier);
	if (!result)
		return false;

	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	params->clip.clipPlane = clipPlane;

	return true;
}

bool RenderClass::RenderToTexture(RenderTextureClass* rendTex, ShaderClass::ShaderParameters* params)
{
	XMMATRIX viewMatrix, displayProjMatrix;

	m_Camera->GetViewMatrix(viewMatrix);

	rendTex->SetRenderTarget(m_Direct3D->GetDeviceContext());
	rendTex->ClearRenderTarget(m_Direct3D->GetDeviceContext(), FOG_COLOR_R, FOG_COLOR_G, FOG_COLOR_B, FOG_COLOR_A);
	rendTex->GetProjectionMatrix(displayProjMatrix);

	bool result = RenderScene(viewMatrix, displayProjMatrix, params);
	if (!result)
		return false;

	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();	

	return true;
}

bool RenderClass::RenderDisplayPlanes(ShaderClass::ShaderParameters* params)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);	
	
	for (auto go : m_AllDisplayPlaneList)
	{
		RenderToTexture(go->m_RenderTexture, params);

		params->matrix.view = viewMatrix;
		params->matrix.projection = projectionMatrix;

		result = go->Render(m_Direct3D->GetDeviceContext(), params);
		if (!result)
			return false;
	}

	return true;
}

bool RenderClass::Render2D(ShaderClass::ShaderParameters* params)
{
	XMMATRIX viewMatrix2D, orthoMatrix;
	bool result;

	m_Direct3D->TurnZBufferOff();
	m_Camera->Get2DViewMatrix(viewMatrix2D);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	params->matrix.view = viewMatrix2D;
	params->matrix.projection = orthoMatrix;

	for (auto go : m_All2DGameObjectList)
	{
		result = go->Render(m_Direct3D->GetDeviceContext(), params);
		if (!result)
			return false;
	}

	for (auto go : m_AllTextClassList)
	{
		result = go->Render(m_Direct3D->GetDeviceContext(), params);
		if (!result)
			return false;
	}

	m_Direct3D->TurnZBufferOn();	

	return true;
}