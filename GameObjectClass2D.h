#ifndef _GAMEOBJECTCLASS2D_H_
#define _GAMEOBJECTCLASS2D_H_

#include <d3d11.h>
#include <directxmath.h>
#include "shaderclass.h"
#include "bitmapclass.h"
#include "settings.h"
using namespace DirectX;

class GameObjectClass2D
{
public:
	GameObjectClass2D();
	GameObjectClass2D(const GameObjectClass2D&);
	~GameObjectClass2D();

	void Initialize(BitmapClass*, ShaderClass*);
	bool Render(ID3D11DeviceContext*, ShaderClass::ShaderParameters*);
	void Shutdown();

	void SetPosition(float, float);
	void SetRotation(float);
	void SetScale(float, float);

public:
	float m_PosX, m_PosY;
	float m_RotZ;
	float m_ScaleX, m_ScaleY;

	BitmapClass* m_BitMap;
	ShaderClass* m_Shader;
	TextureSetClass* m_TexSet;
};

#endif