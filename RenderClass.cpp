#include "RenderClass.h"

RenderClass::RenderClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Frustum = 0;

	m_framesSinceReflectionRender = 0;
	m_renderReflectionImmediately = false;

	m_depthShader = 0;
	m_depthBufferDisplay = 0;
}

RenderClass::~RenderClass()
{
}

bool RenderClass::Initialize(D3DClass* d3d, FrustumClass* frustum, Settings* settings, ShaderClass* shader2D)
{
	m_Direct3D = d3d;
	m_Frustum = frustum;		

	int shadowTexWidth = settings->m_CurrentData.ShadowMapRenderX;
	int shadowTexHeight = settings->m_CurrentData.ShadowMapRenderY;
	int shadowDepth = settings->m_CurrentData.ShadowMapDepth;
	int shadowNear = settings->m_CurrentData.ShadowMapNear;
	float displayWidth = 1.0f;
	float displayHeight = 1.0f;

	m_rendDepth = new RenderTextureClass();
	m_rendDepth->Initialize(d3d->GetDevice(), shadowTexWidth, shadowTexHeight, shadowDepth, shadowNear, 1);

	m_depthBufferDisplay = new DisplayPlaneClass();
	bool result = m_depthBufferDisplay->Initialize(d3d->GetDevice(), displayWidth, displayHeight, m_rendDepth, shader2D, "Display ShadowMap");
	if (!result)
		return false;
	m_depthBufferDisplay->SetPosition(0, 4, 0);

	return true;
}

void RenderClass::Shutdown()
{
	if (m_rendDepth)
	{
		m_rendDepth->Shutdown();
		delete m_rendDepth;
	}

	if (m_depthBufferDisplay)
	{
		m_depthBufferDisplay->Shutdown();
		delete m_depthBufferDisplay;
	}
}

void RenderClass::RenderReflectionNextAvailableFrame()
{
	m_renderReflectionImmediately = true;
}

bool RenderClass::Render(Settings* settings, ShaderClass::ShaderParamsGlobalType* params, SceneClass::SceneDataType* sceneData)
{
	bool result;		

	m_Camera->Render();	

	float fogR = settings->m_CurrentData.FogColorR;
	float fogG = settings->m_CurrentData.FogColorG;
	float fogB = settings->m_CurrentData.FogColorB;
	float fogA = settings->m_CurrentData.FogColorA;

	RenderInfoType renderInfo;
	renderInfo.SceneData = sceneData;
	renderInfo.Params = params;
	renderInfo.Settings = settings;

	if (settings->m_CurrentData.PostProcessingEnabled && sceneData->PostProcessingLayers.size() > 0)
	{
		m_Direct3D->BeginScene(1, 0, 0, 1);

		ClearShaderResources();
		renderInfo.SceneData->PostProcessingLayers.at(0)->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
		renderInfo.SceneData->PostProcessingLayers.at(0)->m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0, 0, 1, fogA);
	}
	else
		m_Direct3D->BeginScene(fogR, fogG, fogB, fogA);

	m_framesSinceShadowMapRender++;
	bool renderShadows = m_framesSinceShadowMapRender >= settings->m_CurrentData.ShadowMapFramesDelay;
	if (renderShadows && settings->m_CurrentData.ShadowsEnabled)
	{
		result = RenderToShadowTexture(&renderInfo);
		if (!result)
			return false;

		for (auto go : sceneData->GoShadowList)
		{
			go->SetShadowTex(m_depthBufferDisplay->m_RenderTexture);
		}

		if (settings->m_CurrentData.ShowShadowMap)
		{
			result = m_depthBufferDisplay->Render(m_Direct3D->GetDeviceContext(), params);
			if (!result)
				return false;
		}

		m_framesSinceShadowMapRender = 0;
	}

	m_framesSinceReflectionRender++;
	bool renderReflection = m_framesSinceReflectionRender >= settings->m_CurrentData.ReflectionFrameDelay || m_renderReflectionImmediately;
	if (settings->m_CurrentData.ReflectionEnabled && renderReflection)
	{
		// Set reflection matrices
		for (auto go : sceneData->GoReflectList)
		{
			m_Camera->RenderReflection(go->m_PosY + go->m_ScaleY);

			XMMATRIX reflectViewMatrix;
			m_Camera->GetReflectionViewMatrix(reflectViewMatrix);
			go->SetReflectionMatrix(reflectViewMatrix);
		}				

		// Refraction comes first
		for (auto go : sceneData->GoRefractList)
		{
			result = RenderToRefractionTexture(go, &renderInfo);
			if (!result)
				return false;
			go->SetRefractionTex();
		}	

		// Then reflection
		for (auto go : sceneData->GoReflectList)
		{
			result = RenderToReflectionTexture(go, &renderInfo);
			if (!result)
				return false;
			go->SetReflectionTex();
		}

		m_framesSinceReflectionRender = 0;
		m_renderReflectionImmediately = false;
	}

	result = SetupDisplayPlanes(&renderInfo);
	if (!result)
		return false;

	vector<string> skippedNames;

	XMMATRIX view, proj;
	m_Camera->GetViewMatrix(view);
	m_Direct3D->GetProjectionMatrix(proj);

	result = RenderScene(&renderInfo, view, proj, skippedNames);
	if (!result)
		return false;

	if (settings->m_CurrentData.PostProcessingEnabled && sceneData->PostProcessingLayers.size() > 0)
	{
		result = RenderPostProcessing(&renderInfo);
		if (!result)
			return false;
	}

	result = Render2D(&renderInfo);
	if (!result)
		return false;

	m_Direct3D->EndScene();

	return true;
}

bool RenderClass::RenderScene(RenderInfoType* renderInfo, XMMATRIX view, XMMATRIX proj, vector<string> skippedNames)
{
	bool result;

	m_Frustum->ConstructFrustum(view, proj, SCREEN_DEPTH);

	m_Direct3D->SetBackCulling(renderInfo->Settings->m_CurrentData.WireframeMode, true);

	result = RenderGameObjects(renderInfo, view, proj, false, skippedNames);
	if (!result)
		return false;

	result = RenderDisplayPlanes(renderInfo, view, proj, skippedNames);
	if (!result)
		return false;

	result = RenderParticleSystems(renderInfo, view, proj, skippedNames);
	if (!result)
		return false;

	result = RenderGameObjects(renderInfo, view, proj, true, skippedNames);
	if (!result)
		return false;

	return true;
}

bool RenderClass::RenderGameObjects(RenderInfoType* renderInfo, XMMATRIX view, XMMATRIX proj, bool transparents, vector<string> skippedNames)
{
	auto list = transparents ? renderInfo->SceneData->GoTransList: renderInfo->SceneData->GoOpaqueList;

	for (auto go : list)
	{
		if (!go->GetRendered())
			continue;

		if (std::find(skippedNames.begin(), skippedNames.end(), go->m_NameIdentifier) != skippedNames.end())
			continue;

		if (go->ModelIsLineList() && !renderInfo->Settings->m_CurrentData.DebugLinesEnabled)
			continue;

		if (renderInfo->Settings->m_CurrentData.FrustumCulling && !m_Frustum->CheckSphere(go->m_PosX, go->m_PosY, go->m_PosZ, go->GetBoundingRadius()))
			continue;		

		if (go->m_BackCullingDisabled)
			m_Direct3D->SetBackCulling(renderInfo->Settings->m_CurrentData.WireframeMode, false);

		go->m_shaderUniformData.matrix.view = view;
		go->m_shaderUniformData.matrix.projection = proj;

		renderInfo->Params->reflection.reflectionMatrix = go->GetReflectionMatrix();
		renderInfo->Params->shadow.usingShadows = renderInfo->Settings->m_CurrentData.ShadowsEnabled && go->IsSubscribedToShadows();

		bool result = go->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params);
		if (!result)
			return false;

		if (go->m_BackCullingDisabled)
			m_Direct3D->SetBackCulling(renderInfo->Settings->m_CurrentData.WireframeMode, true);
	}

	return true;
}

bool RenderClass::RenderParticleSystems(RenderInfoType* renderInfo, XMMATRIX view, XMMATRIX proj, vector<string> skippedNames)
{
	bool result;

	for (auto ps : renderInfo->SceneData->PsList)
	{
		if (std::find(skippedNames.begin(), skippedNames.end(), ps->m_NameIdentifier) != skippedNames.end())
			continue;

		if (ps->m_backCullingDisabled)
			m_Direct3D->SetBackCulling(renderInfo->Settings->m_CurrentData.WireframeMode, false);

		ps->m_shaderUniformData.matrix.view = view;
		ps->m_shaderUniformData.matrix.projection = proj;

		result = ps->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params);
		if (!result)
			return false;

		if (ps->m_backCullingDisabled)
			m_Direct3D->SetBackCulling(renderInfo->Settings->m_CurrentData.WireframeMode, true);
	}

	return true;
}

bool RenderClass::SetupDisplayPlanes(RenderInfoType* renderInfo)
{
	bool result;
	XMMATRIX view;

	for (auto go : renderInfo->SceneData->DisplayPlaneList)
	{
		go->m_Camera->Render();
		go->m_Camera->GetViewMatrix(view);

		vector<string> skippedNames;
		skippedNames.push_back(go->m_NameIdentifier);

		if (go->m_NameIdentifier.starts_with("Display Portal1"))
			skippedNames.push_back("Display Portal2");

		if (go->m_NameIdentifier.starts_with("Display Portal2"))
			skippedNames.push_back("Display Portal1");

		result = RenderToTexture(go->m_RenderTexture, renderInfo, view, skippedNames);
		if (!result)
			return false;
	}

	return true;
}

bool RenderClass::RenderDisplayPlanes(RenderInfoType* renderInfo, XMMATRIX view, XMMATRIX proj, vector<string> skippedNames)
{
	bool result;

	m_Direct3D->SetBackCulling(renderInfo->Settings->m_CurrentData.WireframeMode, false);

	for (auto go : renderInfo->SceneData->DisplayPlaneList)
	{
		if (std::find(skippedNames.begin(), skippedNames.end(), go->m_NameIdentifier) != skippedNames.end())
			continue;

		go->m_shaderUniformData.matrix.view = view;
		go->m_shaderUniformData.matrix.projection = proj;

		result = go->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params);
		if (!result)
			return false;
	}

	m_Direct3D->SetBackCulling(renderInfo->Settings->m_CurrentData.WireframeMode, true);

	return true;
}

bool RenderClass::Render2D(RenderInfoType* renderInfo)
{
	XMMATRIX viewMatrix2D, orthoMatrix;
	bool result;

	m_Direct3D->TurnZBufferOff();
	m_Camera->Get2DViewMatrix(viewMatrix2D);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);	

	for (auto go : renderInfo->SceneData->Go2DList)
	{
		go->m_shaderUniformData.matrix.view = viewMatrix2D;
		go->m_shaderUniformData.matrix.projection = orthoMatrix;

		result = go->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params);
		if (!result)
			return false;
	}

	for (auto go : renderInfo->SceneData->TextList)
	{
		go->m_shaderUniformData.matrix.view = viewMatrix2D;
		go->m_shaderUniformData.matrix.projection = orthoMatrix;

		result = go->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params);
		if (!result)
			return false;
	}

	m_Direct3D->TurnZBufferOn();

	return true;
}

bool RenderClass::RenderSceneDepth(RenderInfoType* renderInfo)
{	
	bool result;

	m_Direct3D->SetBackCulling(false, false);

	for (auto go : renderInfo->SceneData->GoOpaqueList)
	{
		if (!go->GetRendered())
			continue;

		if (go->ModelIsLineList() && !renderInfo->Settings->m_CurrentData.DebugLinesEnabled)
			continue;

	 	go->m_shaderUniformData.matrix.view = renderInfo->Params->shadow.shadowView;
		go->m_shaderUniformData.matrix.projection = renderInfo->Params->shadow.shadowProj;

		result = go->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params, m_depthShader);
		if (!result)
			return false;
	}

	for (auto go : renderInfo->SceneData->GoTransList)
	{
		if (!go->GetRendered())
			continue;

		if (go->ModelIsLineList() && !renderInfo->Settings->m_CurrentData.DebugLinesEnabled)
			continue;

		go->m_shaderUniformData.matrix.view = renderInfo->Params->shadow.shadowView;
		go->m_shaderUniformData.matrix.projection = renderInfo->Params->shadow.shadowProj;

		result = go->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params, m_depthShader);
		if (!result)
			return false;
	}

	for (auto ps : renderInfo->SceneData->PsList)
	{
		ps->m_shaderUniformData.matrix.view = renderInfo->Params->shadow.shadowView;
		ps->m_shaderUniformData.matrix.projection = renderInfo->Params->shadow.shadowProj;

		result = ps->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params, m_depthShader);
		if (!result)
			return false;
	}

	m_Direct3D->SetBackCulling(false, true);

	return true;
}

bool RenderClass::RenderToReflectionTexture(GameObjectClass* go, RenderInfoType* renderInfo)
{
	XMMATRIX reflectViewMatrix, projectionMatrix;

	float height = go->m_PosY + go->m_ScaleY;

	auto clipPlane = go->m_shaderUniformData.clip.clipPlane;

	go->m_shaderUniformData.clip.clipPlane = XMFLOAT4(0.0f, 1.0f, 0.0f, -height + 0.05f);

	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Camera->RenderReflection(height);		
	m_Camera->GetReflectionViewMatrix(reflectViewMatrix);	

	float fogR = renderInfo->Settings->m_CurrentData.FogColorR;
	float fogG = renderInfo->Settings->m_CurrentData.FogColorG;
	float fogB = renderInfo->Settings->m_CurrentData.FogColorB;
	float fogA = renderInfo->Settings->m_CurrentData.FogColorA;

	ClearShaderResources();
	go->m_RendTexReflection->SetRenderTarget(m_Direct3D->GetDeviceContext());
	go->m_RendTexReflection->ClearRenderTarget(m_Direct3D->GetDeviceContext(), fogR, fogG, fogB, fogA);

	vector<string> skippedNames;
	skippedNames.push_back(go->m_NameIdentifier);

	bool result = RenderScene(renderInfo, reflectViewMatrix, projectionMatrix, skippedNames);
	if (!result)
		return false;

	ResetViewport(renderInfo);

	go->m_shaderUniformData.clip.clipPlane = clipPlane;

	return true;
}

bool RenderClass::RenderToRefractionTexture(GameObjectClass* go, RenderInfoType* renderInfo)
{
	XMMATRIX viewMatrix, projectionMatrix;

	float height = go->m_PosY + go->m_ScaleY;

	auto clipPlane = go->m_shaderUniformData.clip.clipPlane;

	go->m_shaderUniformData.clip.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, height + 0.1f);

	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);

	float fogR = renderInfo->Settings->m_CurrentData.FogColorR;
	float fogG = renderInfo->Settings->m_CurrentData.FogColorG;
	float fogB = renderInfo->Settings->m_CurrentData.FogColorB;
	float fogA = renderInfo->Settings->m_CurrentData.FogColorA;

	ClearShaderResources();
	go->m_RendTexRefraction->SetRenderTarget(m_Direct3D->GetDeviceContext());
	go->m_RendTexRefraction->ClearRenderTarget(m_Direct3D->GetDeviceContext(), fogR, fogG, fogB, fogA);

	vector<string> skippedNames;
	skippedNames.push_back(go->m_NameIdentifier);

	bool result = RenderScene(renderInfo, viewMatrix, projectionMatrix, skippedNames);
	if (!result)
		return false;

	ResetViewport(renderInfo);

	go->m_shaderUniformData.clip.clipPlane = clipPlane;

	return true;
}

bool RenderClass::RenderToShadowTexture(RenderInfoType* renderInfo)
{
	ClearShaderResources();
	m_depthBufferDisplay->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_depthBufferDisplay->m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1, 1, 1, 1);

	bool result = RenderSceneDepth(renderInfo);
	if (!result)
		return false;

	ResetViewport(renderInfo);

	return true;
}

bool RenderClass::RenderToTexture(RenderTextureClass* rendTex, RenderInfoType* renderInfo, XMMATRIX view, vector<string> skippedNames)
{
	XMMATRIX displayProjMatrix;

	float fogR = renderInfo->Settings->m_CurrentData.FogColorR;
	float fogG = renderInfo->Settings->m_CurrentData.FogColorG;
	float fogB = renderInfo->Settings->m_CurrentData.FogColorB;
	float fogA = renderInfo->Settings->m_CurrentData.FogColorA;

	ClearShaderResources();
	rendTex->SetRenderTarget(m_Direct3D->GetDeviceContext());
	rendTex->ClearRenderTarget(m_Direct3D->GetDeviceContext(), fogR, fogG, fogB, fogA);
	rendTex->GetProjectionMatrix(displayProjMatrix);

	bool result = RenderScene(renderInfo, view, displayProjMatrix, skippedNames);
	if (!result)
		return false;

	ResetViewport(renderInfo);

	return true;
}

bool RenderClass::RenderPostProcessing(RenderInfoType* renderInfo)
{
	XMMATRIX viewMatrix2D, orthoMatrix;
	bool result;

	m_Direct3D->TurnZBufferOff();
	m_Camera->Get2DViewMatrix(viewMatrix2D);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	int count = renderInfo->SceneData->PostProcessingLayers.size();
	for (int i = 0; i < count; i++)
	{
		ClearShaderResources();
		if (i == count - 1)
		{			
			m_Direct3D->SetBackBufferRenderTarget();
			m_Direct3D->ResetViewport();
		}
		else
		{
			DisplayPlaneClass* dpNext = renderInfo->SceneData->PostProcessingLayers.at(i+1);
			dpNext->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
			dpNext->m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1, 1, 0, 1);
		}

		DisplayPlaneClass* dp = renderInfo->SceneData->PostProcessingLayers.at(i);

		dp->m_shaderUniformData.matrix.view = viewMatrix2D;
		dp->m_shaderUniformData.matrix.projection = orthoMatrix;

		result = dp->Render(m_Direct3D->GetDeviceContext(), renderInfo->Params);
		if (!result)
			return false;
	}

	return true;
}

void RenderClass::SetDepthShader(ShaderClass* shader)
{
	m_depthShader = shader;
}

void RenderClass::SetCurrentCamera(CameraClass* cam)
{
	m_Camera = cam;
}

void RenderClass::ResetViewport(RenderInfoType* renderInfo)
{
	if (renderInfo->Settings->m_CurrentData.PostProcessingEnabled && renderInfo->SceneData->PostProcessingLayers.size() > 0)
	{
		renderInfo->SceneData->PostProcessingLayers.at(0)->m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
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