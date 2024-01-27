#ifndef _RENDERCLASS_H_
#define _RENDERCLASS_H_

#include "d3dclass.h"
#include "cameraclass.h"
#include "textclass.h"
#include <vector>
#include "GameObjectClass.h"
#include "GameObjectClass2D.h"
#include "FrustumClass.h"
#include "rendertextureclass.h"
#include "displayplaneclass.h"
#include "ParticleSystemClass.h"
using std::vector;
using std::string;
using std::unordered_map;
using std::any;
using std::any_cast;

class RenderClass
{
public:
	RenderClass();
	~RenderClass();

	bool Initialize(D3DClass*, CameraClass*, FrustumClass*);
	void Shutdown();

	void AddGameObject(GameObjectClass*);
	void AddGameObject2D(GameObjectClass2D*);
	void AddTextClass(TextClass*);
	void AddDisplayPlane(DisplayPlaneClass*);
	void AddParticleSystem(ParticleSystemClass* ps);
	void SubscribeToReflection(ID3D11Device*, GameObjectClass*, int, int);
	void SubscribeToRefraction(ID3D11Device*, GameObjectClass*, int, int);
	void SubscribeToShadow(GameObjectClass*, int);

	bool Render(Settings*, ShaderClass::ShaderParameters*);
	void RenderReflectionNextAvailableFrame();

	void SetDepthShader(ShaderClass*);
	void SetShadowMapDisplayPlane(DisplayPlaneClass*);
	void SetPostProcessingDisplayPlanes(DisplayPlaneClass*, DisplayPlaneClass*, DisplayPlaneClass*);

	void ClearShaderResources();

private:
	bool RenderScene(Settings*, XMMATRIX, XMMATRIX, ShaderClass::ShaderParameters*, string skipGO = "-N/A-");
	bool RenderGameObjects(ShaderClass::ShaderParameters*, Settings*, string);
	bool RenderDisplayPlanes(ShaderClass::ShaderParameters*, string);
	bool SetupDisplayPlanes(ShaderClass::ShaderParameters*, Settings*);
	bool RenderParticleSystems(ShaderClass::ShaderParameters*, Settings*,  string);
	bool RenderToRefractionTexture(ShaderClass::ShaderParameters* params, Settings* settings);
	bool SetUpDisplayPlanes(ShaderClass::ShaderParameters* params, Settings* settings, string skipGO);
	bool Render2D(ShaderClass::ShaderParameters*);	

	bool RenderSceneDepth(ShaderClass::ShaderParameters*);
	bool RenderToTexture(RenderTextureClass*, ShaderClass::ShaderParameters*, Settings*);
	bool RenderToReflectionTexture(GameObjectClass*, ShaderClass::ShaderParameters*, Settings*);
	bool RenderToRefractionTexture(GameObjectClass*, ShaderClass::ShaderParameters*, Settings*);
	bool RenderToShadowTexture(ShaderClass::ShaderParameters*, Settings*);	

	bool RenderPostProcessing(ShaderClass::ShaderParameters*, Settings*);

	void ResetViewport(Settings*);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	FrustumClass* m_Frustum;

	vector<GameObjectClass*> m_AllGameObjectList;
	vector<GameObjectClass*> m_ReflectionList;
	vector<GameObjectClass*> m_RefractionList;
	vector<GameObjectClass*> m_ShadowList;

	vector<GameObjectClass2D*> m_All2DGameObjectList;
	vector<TextClass*> m_AllTextClassList;
	vector<DisplayPlaneClass*> m_AllDisplayPlaneList;
	vector<ParticleSystemClass*> m_AllParticleSystemList;

	ShaderClass* m_depthShader;
	DisplayPlaneClass* m_shadowMapDisplay;
	DisplayPlaneClass* m_ppFirstPassBlurDisplay;
	DisplayPlaneClass* m_ppSecondPassBlurDisplay;
	DisplayPlaneClass* m_ppThirdPassFilterDisplay;

	int m_framesSinceReflectionRender;
	bool m_renderReflectionImmediately;
};

#endif