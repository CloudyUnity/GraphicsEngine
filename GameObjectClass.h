#ifndef _GAMEOBJECTCLASS_H_
#define _GAMEOBJECTCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include "TextureShaderClass.h"
#include "ModelClass.h"
#include "TextureSetClass.h"
#include <string>
#include "RenderTextureClass.h"
#include "Constants.h"
using namespace DirectX;

class GameObjectClass
{
public:
	GameObjectClass();
	GameObjectClass(const GameObjectClass&);
	~GameObjectClass();

	void Initialize(ModelClass*, ShaderClass*, TextureSetClass*, std::string);
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, unordered_map<string, any>);
	void Shutdown();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetScale(float, float, float);

	void SubscribeToReflection(ID3D11Device*, int, int);
	void SubscribeToRefraction(ID3D11Device*, int, int);
	void SetReflectionTex();
	void SetRefractionTex();

	float GetBoundingRadius();

public:
	float m_PosX, m_PosY, m_PosZ;
	float m_RotX, m_RotY, m_RotZ;
	float m_ScaleX, m_ScaleY, m_ScaleZ;
	std::string m_NameIdentifier;

	RenderTextureClass* m_RendTexReflection, * m_RendTexRefraction;

private:
	float m_boundingRadius;

	ModelClass* m_Model;
	ShaderClass* m_Shader;
	TextureSetClass* m_Textures;

	int m_texSetReflectionNum;
	int m_texSetRefractionNum;
};

#endif