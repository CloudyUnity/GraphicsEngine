#ifndef _SCENECLASS_H_
#define _SCENECLASS_H_

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "shaderclass.h"
#include "lightclass.h"
#include "bitmapclass.h"
#include "fontclass.h"
#include "textclass.h"
#include "InputClass.h"
#include "GameObjectClass.h"
#include "GameObjectClass2D.h"
#include "rendertextureclass.h"
#include "displayplaneclass.h"
#include "ParticleSystemClass.h"

class RenderClass;

class SceneClass : public IShutdown
{
public:	
	struct SceneDataType
	{
		vector<GameObjectClass*> GoOpaqueList;
		vector<GameObjectClass*> GoTransList;
		vector<GameObjectClass*> GoReflectList;
		vector<GameObjectClass*> GoRefractList;
		vector<GameObjectClass*> GoShadowList;

		vector<GameObjectClass2D*> Go2DList;
		vector<TextClass*> TextList;				

		vector<ParticleSystemClass*> PsList;

		vector<DisplayPlaneClass*> DisplayPlaneList;
		vector<DisplayPlaneClass*> PostProcessingLayers;
	};

	SceneClass();

    bool InitializeMembers(Settings* settings, D3DClass* d3d, RenderClass* renderClass);
	virtual bool InitializeScene(HWND hwnd);

    bool ParticlesFrame(float frameTime);
	virtual bool Frame(InputClass*, float frameTime);
	virtual bool LateFrame(InputClass* input, float frameTime);
	virtual void SetParameters(ShaderClass::ShaderParameters*);
	virtual void OnSwitchTo();
	void Shutdown() override;

	SceneDataType* GetSceneData();

	bool m_InitializedScene;

protected:
	bool CreateModel(HWND, ModelClass**, const char*);
	bool CreateShader(HWND, ShaderClass**, const char*, const char*, bool clampSamplerMode = false);
	void CreateGameObject(ModelClass*, ShaderClass*, TextureSetClass*, bool, const char*, GameObjectClass** ptr = nullptr);
	void CreateGameObject2D(BitmapClass*, ShaderClass*, GameObjectClass2D** ptr = nullptr);
	bool CreateText(TextClass** ptr, ShaderClass* shader, FontClass*, int maxLength);
	void CreateTexSet(TextureSetClass** ptr);
	bool CreateBitmap(BitmapClass**, const char*);
	bool CreateParticleSystem(ParticleSystemClass**, ParticleSystemClass::ParticleSystemData, ShaderClass*, TextureSetClass*, const char*);
	bool CreateRenderTexture(RenderTextureClass**, ID3D11Device*, int, int, float, float, int);
	bool CreateDisplayPlane(DisplayPlaneClass**, ID3D11Device*, float, float, RenderTextureClass*, ShaderClass*, const char*, bool postProcess = false, CameraClass* cam = nullptr);

	void SubscribeToReflection(GameObjectClass* goPtr, int, int);
	void SubscribeToRefraction(GameObjectClass* goPtr, int, int);
	void SubscribeToShadow(GameObjectClass* goPtr, int);

	Settings* m_settings;
	D3DClass* m_Direct3D;
	RenderClass* m_RenderClass;
	vector<ParticleSystemClass*> m_renderedPSList;
	SceneDataType* m_sceneData;

	vector<IShutdown*> m_loadedAssetsList;
};

#endif