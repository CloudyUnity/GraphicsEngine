#include "GameObjectClass2D.h"

GameObjectClass2D::GameObjectClass2D()
{
	m_BitMap = 0;
	m_Shader = 0;

	SetScale(1, 1);
}

GameObjectClass2D::GameObjectClass2D(const GameObjectClass2D&)
{
}

GameObjectClass2D::~GameObjectClass2D()
{
}

void GameObjectClass2D::Initialize(BitmapClass* bitmap, ShaderClass* shaders)
{
	m_BitMap = bitmap;
	m_Shader = shaders;

	m_TexSet = new TextureSetClass;
}

bool GameObjectClass2D::Render(ID3D11DeviceContext* deviceContext, XMMATRIX viewMatrix, XMMATRIX projMatrix, unordered_map<string, any> args)
{
	/*float posX = fmodf(m_PosX, SCREEN_X / m_ScaleX);
	float posY = fmodf(m_PosY, SCREEN_Y / m_ScaleY);
	if (posX < 0)
		posX += SCREEN_X;
	if (posY < 0)
		posY += SCREEN_Y;*/

	XMMATRIX translateMatrix = XMMatrixTranslation(m_PosX, m_PosY, 0); // TO FIX (Look at BitMapClass.cpp to see how to properly calculate position. Then place into translation matrix)
	XMMATRIX scaleMatrix = XMMatrixScaling(m_ScaleX, m_ScaleY, 1.0f);
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(0, 0, m_RotZ * 0.0174532925f);

	XMMATRIX srMatrix = XMMatrixMultiply(rotateMatrix, scaleMatrix);
	XMMATRIX worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_BitMap->SetRenderLocation(0, 0);
	m_BitMap->Render(deviceContext);	

	m_TexSet->Add(m_BitMap->GetTexture(), 0);

	return m_Shader->Render(deviceContext, m_BitMap->GetIndexCount(), worldMatrix, viewMatrix, projMatrix, m_TexSet, args);
}

void GameObjectClass2D::Shutdown()
{
	if (m_BitMap) {
		m_BitMap = 0;
	}

	if (m_Shader) {
		m_Shader = 0;
	}

	if (m_TexSet)
	{
		m_TexSet->Shutdown();
		delete m_TexSet;
		m_TexSet = 0;
	}
}

void GameObjectClass2D::SetPosition(float x, float y)
{
	m_PosX = x;
	m_PosY = y;
}

void GameObjectClass2D::SetRotation(float z)
{
	m_RotZ = z;
}

void GameObjectClass2D::SetScale(float x, float y)
{
	m_ScaleX = x;
	m_ScaleY = y;
}
