#ifndef _GAMEOBJECTCLASS_H_
#define _GAMEOBJECTCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include "shaderclass.h"
#include "ModelClass.h"
#include "TextureSetClass.h"
#include <string>
#include "RenderTextureClass.h"
#include "settings.h"
#include "IShutdown.h"
#include "ShaderClass.h"
using namespace DirectX;

class GameObjectClass : public IShutdown
{
public:
	GameObjectClass();
	~GameObjectClass();

	void Initialize(ModelClass*, ShaderClass*, TextureSetClass*, std::string);
	bool Render(ID3D11DeviceContext*, ShaderClass::ShaderParamsGlobalType* params, ShaderClass* overwriteShader = nullptr);
	void Shutdown() override;

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetScale(float, float, float);
	void SetScale(float);

	void SubscribeToReflection(ID3D11Device*, int, int);
	void SubscribeToRefraction(ID3D11Device*, int, int);
	void SubscribeToShadows(int);
	void SetReflectionTex();
	void SetRefractionTex();
	void SetShadowTex(RenderTextureClass*);
	void SetReflectionMatrix(XMMATRIX);
	XMMATRIX GetReflectionMatrix();

	bool IsSubscribedToShadows();

	void SetBillBoarding(bool);
	void SetBackCulling(bool);
	void SetRendered(bool);

	float GetBoundingRadius();
	bool GetRendered();
	bool ModelIsLineList();

public:
	float m_PosX, m_PosY, m_PosZ;
	float m_RotX, m_RotY, m_RotZ;
	float m_ScaleX, m_ScaleY, m_ScaleZ;
	std::string m_NameIdentifier;

	RenderTextureClass* m_RendTexReflection, * m_RendTexRefraction;

	bool m_BillboardingEnabled;
	bool m_BackCullingDisabled;

	ShaderClass::ShaderParamsObjectType m_shaderUniformData;

private:
	float m_boundingRadius;

	ModelClass* m_Model;
	ShaderClass* m_Shader;
	TextureSetClass* m_Textures;

	int m_texSetReflectionNum;
	int m_texSetRefractionNum;
	int m_texSetShadowNum;
	XMMATRIX m_reflectMatrix;	

	bool m_rendered;
};

#endif