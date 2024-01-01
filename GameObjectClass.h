#ifndef _GAMEOBJECTCLASS_H_
#define _GAMEOBJECTCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include "TextureShaderClass.h"
#include "ModelClass.h"
using namespace DirectX;

class GameObjectClass
{
public:
	GameObjectClass();
	GameObjectClass(const GameObjectClass&);
	~GameObjectClass();

	void Initialize(ModelClass*, TextureShaderClass*);
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, unordered_map<string, any>);
	void Shutdown();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetScale(float, float, float);

	float GetBoundingRadius();

public:
	float m_PosX, m_PosY, m_PosZ;
	float m_RotX, m_RotY, m_RotZ;
	float m_ScaleX, m_ScaleY, m_ScaleZ;

private:
	float m_boundingRadius;

	ModelClass* m_Model;
	TextureShaderClass* m_Shader;
};

#endif