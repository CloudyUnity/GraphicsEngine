#include "RenderClass.h"

RenderClass::RenderClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Frustum = 0;

	m_framesSinceReflectionRender = 0;
	m_renderReflectionImmediately = false;

	m_depthShader = 0;
	m_shadowMapDisplay = 0;
	m_ppFirstPassBlurDisplay = 0;
	m_ppSecondPassBlurDisplay = 0;
	m_ppThirdPassFilterDisplay= 0;
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

void RenderClass::AddParticleSystem(ParticleSystemClass* ps)
{
	m_AllParticleSystemList.push_back(ps);
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

void RenderClass::SubscribeToShadow(GameObjectClass* go, int texSetNum)
{
	m_ShadowList.push_back(go);

	go->SubscribeToShadows(texSetNum);
}

void RenderClass::RenderReflectionNextAvailableFrame()
{
	m_renderReflectionImmediately = true;
}

bool RenderClass::Render(Settings* settings, ShaderClass::ShaderParameters* params)
{
	XMMATRIX viewMatrix, projectionMatrix, reflectViewMatrix;
	bool result;		

	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Frustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);

	float fogR = settings->m_CurrentData.FogColorR;
	float fogG = settings->m_CurrentData.FogColorG;
	float fogB = settings->m_CurrentData.FogColorB;
	float fogA = settings->m_CurrentData.FogColorA;

	if (settings->m_CurrentData.PostProcessingEnabled)
	{
		m_Direct3D->BeginScene(1, 0, 0, 1);

		ClearShaderResources();
		m_ppFirstPassBlurDisplay->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
		m_ppFirstPassBlurDisplay->m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0, 0, 1, fogA);
	}
	else
		m_Direct3D->BeginScene(fogR, fogG, fogB, fogA);

	m_framesSinceReflectionRender++;
	bool renderReflection = m_framesSinceReflectionRender >= settings->m_CurrentData.ReflectionFrameDelay || m_renderReflectionImmediately;
	if (settings->m_CurrentData.ReflectionEnabled && renderReflection)
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
			result = RenderToRefractionTexture(go, params, settings);
			if (!result)
				return false;
			go->SetRefractionTex();
		}	

		// Then reflection
		for (auto go : m_ReflectionList)
		{
			result = RenderToReflectionTexture(go, params, settings);
			if (!result)
				return false;
			go->SetReflectionTex();
		}

		m_framesSinceReflectionRender = 0;
		m_renderReflectionImmediately = false;
	}

	if (settings->m_CurrentData.ShadowsEnabled)
	{
		result = RenderToShadowTexture(params, settings);
		if (!result)
			return false;

		for (auto go : m_ShadowList)
		{			
			go->SetShadowTex(m_shadowMapDisplay->m_RenderTexture);
		}

		if (settings->m_CurrentData.ShowShadowMap)
		{
			params->matrix.view = viewMatrix;
			params->matrix.projection = projectionMatrix;

			result = m_shadowMapDisplay->Render(m_Direct3D->GetDeviceContext(), params);
			if (!result)
				return false;
		}
	}

	result = SetupDisplayPlanes(params, settings);
	if (!result)
		return false;

	result = RenderScene(settings, viewMatrix, projectionMatrix, params);
	if (!result)
		return false;

	if (settings->m_CurrentData.PostProcessingEnabled)
	{
		result = RenderPostProcessing(params, settings);
		if (!result)
			return false;
	}

	result = Render2D(params);
	if (!result)
		return false;

	m_Direct3D->EndScene();

	return true;
}

bool RenderClass::RenderScene(Settings* settings, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ShaderClass::ShaderParameters* params, string skipGO)
{
	bool result;

	params->matrix.view = viewMatrix;
	params->matrix.projection = projectionMatrix;

	result = RenderGameObjects(params, settings, skipGO);
	if (!result)
		return false;

	result = RenderDisplayPlanes(params, skipGO);
	if (!result)
		return false;

	result = RenderParticleSystems(params, settings, skipGO);
	if (!result)
		return false;

	return true;
}

bool RenderClass::RenderGameObjects(ShaderClass::ShaderParameters* params, Settings* settings, string skipGO)
{
	auto savedParams = *params;

	for (auto go : m_AllGameObjectList)
	{
		if (go->m_NameIdentifier == skipGO)
			continue;

		if (settings->m_CurrentData.FrustumCulling && !m_Frustum->CheckSphere(go->m_PosX, go->m_PosY, go->m_PosZ, go->GetBoundingRadius()))
			continue;

		if (go->m_NameIdentifier == "Madeline2")
			params->textureTranslation.timeMultiplier = 0.1f;
		else if (go->m_NameIdentifier == "Water")
			params->textureTranslation.timeMultiplier = (0.1f / go->m_ScaleX) / go->m_ScaleZ;

		if (go->m_NameIdentifier == "IcosphereTrans")
			params->alpha.alphaBlend = 0.1f;

		if (go->m_NameIdentifier == "IcosphereBig")
			params->clip.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 2.5f);

		if (go->m_BackCullingDisabled)
			m_Direct3D->SetBackCulling(settings->m_CurrentData.WireframeMode, false);

		if (go->m_NameIdentifier == "Skybox" && !settings->m_CurrentData.SkyboxEnabled)
			continue;

		params->reflection.reflectionMatrix = go->GetReflectionMatrix();
		params->shadow.usingShadows = settings->m_CurrentData.ShadowsEnabled && go->IsSubscribedToShadows();

		bool result = go->Render(m_Direct3D->GetDeviceContext(), params);
		if (!result)
			return false;

		if (go->m_BackCullingDisabled)
			m_Direct3D->SetBackCulling(settings->m_CurrentData.WireframeMode, true);

		*params = savedParams;
	}

	return true;
}

bool RenderClass::RenderParticleSystems(ShaderClass::ShaderParameters* params, Settings* settings, string skipGO)
{
	bool result;

	for (auto ps : m_AllParticleSystemList)
	{
		if (ps->m_NameIdentifier == skipGO)
			continue;

		if (ps->m_backCullingDisabled)
			m_Direct3D->SetBackCulling(settings->m_CurrentData.WireframeMode, false);

		result = ps->Render(m_Direct3D->GetDeviceContext(), params);
		if (!result)
			return false;

		if (ps->m_backCullingDisabled)
			m_Direct3D->SetBackCulling(settings->m_CurrentData.WireframeMode, true);
	}

	return true;
}

bool RenderClass::SetupDisplayPlanes(ShaderClass::ShaderParameters* params, Settings* settings)
{
	bool result;

	for (auto go : m_AllDisplayPlaneList)
	{
		result = RenderToTexture(go->m_RenderTexture, params, settings);
		if (!result)
			return false;
	}

	return true;
}

bool RenderClass::RenderDisplayPlanes(ShaderClass::ShaderParameters* params, string skipGO)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	for (auto go : m_AllDisplayPlaneList)
	{
		if (go->m_NameIdentifier == skipGO)
			continue;

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

bool RenderClass::RenderSceneDepth(ShaderClass::ShaderParameters* params)
{	
	bool result;

	params->matrix.view = params->shadow.shadowView;
	params->matrix.projection = params->shadow.shadowProj;

	auto savedParams = *params;	

	m_Direct3D->SetBackCulling(false, false);

	for (auto go : m_AllGameObjectList)
	{
		if (go->m_NameIdentifier == "Skybox")
			continue;

		if (go->m_NameIdentifier == "IcosphereBig")
			params->clip.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 2.5f);

		result = go->Render(m_Direct3D->GetDeviceContext(), params, m_depthShader);
		if (!result)
			return false;

		*params = savedParams;
	}

	for (auto ps : m_AllParticleSystemList)
	{
		result = ps->Render(m_Direct3D->GetDeviceContext(), params, m_depthShader);
		if (!result)
			return false;
	}

	m_Direct3D->SetBackCulling(false, true);

	return true;
}

bool RenderClass::RenderToReflectionTexture(GameObjectClass* go, ShaderClass::ShaderParameters* params, Settings* settings)
{
	XMMATRIX reflectViewMatrix, projectionMatrix;

	float height = go->m_PosY + go->m_ScaleY;

	auto clipPlane = params->clip.clipPlane;

	params->clip.clipPlane = XMFLOAT4(0.0f, 1.0f, 0.0f, -height + 0.05f);

	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Camera->RenderReflection(height);		
	m_Camera->GetReflectionViewMatrix(reflectViewMatrix);	

	float fogR = settings->m_CurrentData.FogColorR;
	float fogG = settings->m_CurrentData.FogColorG;
	float fogB = settings->m_CurrentData.FogColorB;
	float fogA = settings->m_CurrentData.FogColorA;

	ClearShaderResources();
	go->m_RendTexReflection->SetRenderTarget(m_Direct3D->GetDeviceContext());
	go->m_RendTexReflection->ClearRenderTarget(m_Direct3D->GetDeviceContext(), fogR, fogG, fogB, fogA);

	bool result = RenderScene(settings, reflectViewMatrix, projectionMatrix, params, go->m_NameIdentifier);
	if (!result)
		return false;

	ResetViewport(settings);

	params->clip.clipPlane = clipPlane;

	return true;
}

bool RenderClass::RenderToRefractionTexture(GameObjectClass* go, ShaderClass::ShaderParameters* params, Settings* settings)
{
	XMMATRIX viewMatrix, projectionMatrix;

	float height = go->m_PosY + go->m_ScaleY;

	auto clipPlane = params->clip.clipPlane;

	params->clip.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, height + 0.1f);

	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);

	float fogR = settings->m_CurrentData.FogColorR;
	float fogG = settings->m_CurrentData.FogColorG;
	float fogB = settings->m_CurrentData.FogColorB;
	float fogA = settings->m_CurrentData.FogColorA;

	ClearShaderResources();
	go->m_RendTexRefraction->SetRenderTarget(m_Direct3D->GetDeviceContext());
	go->m_RendTexRefraction->ClearRenderTarget(m_Direct3D->GetDeviceContext(), fogR, fogG, fogB, fogA);

	bool result = RenderScene(settings, viewMatrix, projectionMatrix, params, go->m_NameIdentifier);
	if (!result)
		return false;

	ResetViewport(settings);

	params->clip.clipPlane = clipPlane;

	return true;
}

bool RenderClass::RenderToShadowTexture(ShaderClass::ShaderParameters* params, Settings* settings)
{
	ClearShaderResources();
	m_shadowMapDisplay->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_shadowMapDisplay->m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1, 1, 1, 1);

	bool result = RenderSceneDepth(params);
	if (!result)
		return false;

	ResetViewport(settings);

	return true;
}

bool RenderClass::RenderToTexture(RenderTextureClass* rendTex, ShaderClass::ShaderParameters* params, Settings* settings)
{
	XMMATRIX viewMatrix, displayProjMatrix;

	m_Camera->GetViewMatrix(viewMatrix);

	float fogR = settings->m_CurrentData.FogColorR;
	float fogG = settings->m_CurrentData.FogColorG;
	float fogB = settings->m_CurrentData.FogColorB;
	float fogA = settings->m_CurrentData.FogColorA;

	ClearShaderResources();
	rendTex->SetRenderTarget(m_Direct3D->GetDeviceContext());
	rendTex->ClearRenderTarget(m_Direct3D->GetDeviceContext(), fogR, fogG, fogB, fogA);
	rendTex->GetProjectionMatrix(displayProjMatrix);

	bool result = RenderScene(settings, viewMatrix, displayProjMatrix, params);
	if (!result)
		return false;

	ResetViewport(settings);

	return true;
}

bool RenderClass::RenderPostProcessing(ShaderClass::ShaderParameters* params, Settings* settings)
{
	XMMATRIX viewMatrix2D, orthoMatrix;
	bool result;

	m_Direct3D->TurnZBufferOff();
	m_Camera->Get2DViewMatrix(viewMatrix2D);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	params->matrix.view = viewMatrix2D;
	params->matrix.projection = orthoMatrix;

	ClearShaderResources();
	m_ppSecondPassBlurDisplay->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_ppSecondPassBlurDisplay->m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1, 1, 0, 1);

	params->blur.blurMode = settings->m_CurrentData.BlurEnabled ? 0 : -1;
	result = m_ppFirstPassBlurDisplay->Render(m_Direct3D->GetDeviceContext(), params);
	if (!result)
		return false;	

	ClearShaderResources();
	m_ppThirdPassFilterDisplay->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_ppThirdPassFilterDisplay->m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1, 1, 0, 1);

	params->blur.blurMode = settings->m_CurrentData.BlurEnabled ? 1 : -1;
	result = m_ppSecondPassBlurDisplay->Render(m_Direct3D->GetDeviceContext(), params);
	if (!result)
		return false;

	ClearShaderResources();
	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	result = m_ppThirdPassFilterDisplay->Render(m_Direct3D->GetDeviceContext(), params);
	if (!result)
		return false;

	if (!result)
		return false;

	return true;
}

void RenderClass::SetDepthShader(ShaderClass* shader)
{
	m_depthShader = shader;
}

void RenderClass::SetShadowMapDisplayPlane(DisplayPlaneClass* display)
{
	m_shadowMapDisplay = display;
}

void RenderClass::SetPostProcessingDisplayPlanes(DisplayPlaneClass* first, DisplayPlaneClass* second, DisplayPlaneClass* third)
{
	m_ppFirstPassBlurDisplay = first;
	m_ppSecondPassBlurDisplay = second;
	m_ppThirdPassFilterDisplay = third;
}

void RenderClass::ResetViewport(Settings* settings)
{
	if (settings->m_CurrentData.PostProcessingEnabled)
	{
		m_ppFirstPassBlurDisplay->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
		return;
	}

	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();
}

void RenderClass::ClearShaderResources()
{
	ID3D11ShaderResourceView* nullSRV[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	m_Direct3D->GetDeviceContext()->PSSetShaderResources(0, 6, nullSRV);
}