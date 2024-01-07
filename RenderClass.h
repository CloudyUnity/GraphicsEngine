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

	bool Render(unordered_map<string, any>);
	bool RenderScene(XMMATRIX, XMMATRIX, unordered_map<string, any>, string skipGO = "-N/A-");
	bool RenderToTexture(RenderTextureClass*, unordered_map<string, any>);
	bool RenderToReflectionTexture(RenderTextureClass*, float, unordered_map<string, any>, string);
	bool RenderToRefractionTexture(RenderTextureClass*, float, unordered_map<string, any>, string);
	bool RenderDisplayPlanes(unordered_map<string, any>);
	bool Render2D(unordered_map<string, any>);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	FrustumClass* m_Frustum;

	vector<GameObjectClass*> m_AllGameObjectList;
	vector<GameObjectClass*> m_ReflectionList;
	vector<GameObjectClass*> m_RefractionList;

	vector<GameObjectClass2D*> m_All2DGameObjectList;
	vector<TextClass*> m_AllTextClassList;
	vector<DisplayPlaneClass*> m_AllDisplayPlaneList;
};

#endif