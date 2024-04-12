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
#include "SceneClass.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::any;
using std::any_cast;

class RenderClass : IShutdown
{
public:
	struct RenderInfoType
	{
		SceneClass::SceneDataType* SceneData;
		ShaderClass::ShaderParameters* Params;
		Settings* Settings;
	};

	RenderClass();
	~RenderClass();

	bool Initialize(D3DClass*, FrustumClass*, Settings* settings, ShaderClass* shader2D);
	void Shutdown() override;

	bool Render(Settings*, ShaderClass::ShaderParameters*, SceneClass::SceneDataType* sceneData);
	void RenderReflectionNextAvailableFrame();

	void SetDepthShader(ShaderClass*);
	void AddPostProcessingLayer(DisplayPlaneClass*);
	void SetCurrentCamera(CameraClass* cam);

	void ClearShaderResources();

private:
	bool RenderScene(RenderInfoType*, XMMATRIX, XMMATRIX, vector<string> skippedNames);
	bool RenderGameObjects(RenderInfoType*, bool transparents, vector<string> skippedNames);
	bool RenderDisplayPlanes(RenderInfoType*, vector<string> skippedNames);
	bool SetupDisplayPlanes(RenderInfoType*);
	bool RenderParticleSystems(RenderInfoType*, vector<string> skippedNames);
	bool RenderToRefractionTexture(RenderInfoType*);
	bool SetUpDisplayPlanes(RenderInfoType*, vector<string> skippedNames);
	bool Render2D(RenderInfoType*);

	bool RenderSceneDepth(RenderInfoType*);
	bool RenderToTexture(RenderTextureClass*, RenderInfoType*, XMMATRIX, vector<string> skippedNames);
	bool RenderToReflectionTexture(GameObjectClass*, RenderInfoType*);
	bool RenderToRefractionTexture(GameObjectClass*, RenderInfoType*);
	bool RenderToShadowTexture(RenderInfoType*);

	bool RenderPostProcessing(RenderInfoType*);

	void ResetViewport(Settings*);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	FrustumClass* m_Frustum;

	ShaderClass* m_depthShader;
	RenderTextureClass* m_rendDepth; // Shutdown
	DisplayPlaneClass* m_depthBufferDisplay;
	vector<DisplayPlaneClass*> m_postProcessingLayers;

	int m_framesSinceReflectionRender;
	int m_framesSinceShadowMapRender;
	bool m_renderReflectionImmediately;
};

#endif