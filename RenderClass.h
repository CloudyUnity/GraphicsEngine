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
using std::vector;
using std::string;
using std::unordered_map;
using std::any;
using std::any_cast;

class RenderClass
{
public:
	RenderClass();
	RenderClass(const RenderClass&);
	~RenderClass();

	bool Initialize(D3DClass*, CameraClass*, FrustumClass*);
	void Shutdown();

	void AddGameObject(GameObjectClass*);
	void AddGameObject2D(GameObjectClass2D*);
	void AddTextClass(TextClass*);
	void AddDisplayPlane(DisplayPlaneClass*);
	void SubscribeToReflection(ID3D11Device*, GameObjectClass*, int, int);
	void SubscribeToRefraction(ID3D11Device*, GameObjectClass*, int, int);
	void SubscribeToShadow(GameObjectClass*, int);

	bool Render(ShaderClass::ShaderParameters*);
	void RenderReflectionNextAvailableFrame();

	void SetDepthShader(ShaderClass*);
	void SetShadowMapDisplayPlane(DisplayPlaneClass*);

private:
	bool RenderScene(XMMATRIX, XMMATRIX, ShaderClass::ShaderParameters*, string skipGO = "-N/A-");
	bool RenderSceneDepth(ShaderClass::ShaderParameters*);
	bool RenderToTexture(RenderTextureClass*, ShaderClass::ShaderParameters*);
	bool RenderToReflectionTexture(GameObjectClass*, ShaderClass::ShaderParameters*);
	bool RenderToRefractionTexture(GameObjectClass*, ShaderClass::ShaderParameters*);
	bool RenderToShadowTexture(ShaderClass::ShaderParameters*);
	bool RenderDisplayPlanes(ShaderClass::ShaderParameters*);
	bool Render2D(ShaderClass::ShaderParameters*);	

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

	ShaderClass* m_depthShader;
	DisplayPlaneClass* m_shadowMapDisplay;

	int m_framesSinceReflectionRender;
	bool m_renderReflectionImmediately;
};

#endif