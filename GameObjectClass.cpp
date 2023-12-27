#include "GameObjectClass.h"

GameObjectClass::GameObjectClass()
{
	m_Model = 0;
	m_Shader = 0;

	SetScale(1, 1, 1);
}

GameObjectClass::GameObjectClass(const GameObjectClass&)
{
}

GameObjectClass::~GameObjectClass()
{
}

void GameObjectClass::Initialize(ModelClass* model, TextureShaderClass* shaders)
{
	m_Model = model;
	m_Shader = shaders;
}

bool GameObjectClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX viewMatrix, XMMATRIX projMatrix, unordered_map<string, any> arguments)
{
	XMMATRIX scaleMatrix = XMMatrixScaling(m_ScaleX, m_ScaleY, m_ScaleZ);
	XMMATRIX translateMatrix = XMMatrixTranslation(m_PosX, m_PosY, m_PosZ);
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(m_RotX * 0.0174532925f, m_RotY * 0.0174532925f, m_RotZ * 0.0174532925f); // ???
	XMMATRIX srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
	XMMATRIX worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->RenderBuffers(deviceContext);

	return m_Shader->Render(deviceContext, m_Model->GetIndexCount(), worldMatrix, viewMatrix, projMatrix, m_Model->m_Textures, m_Model->m_TextureCount, arguments);
}

void GameObjectClass::Shutdown() 
{
	if (m_Model) {
		m_Model = 0;
	}

	if (m_Shader) {
		m_Shader = 0;
	}
}

void GameObjectClass::SetPosition(float  x, float y, float z)
{
	m_PosX = x;
	m_PosY = y;
	m_PosZ = z;
}

void GameObjectClass::SetRotation(float x, float y, float z)
{
	m_RotX = x;
	m_RotY = y;
	m_RotZ = z;
}

void GameObjectClass::SetScale(float x, float y, float z)
{
	m_ScaleX = x;
	m_ScaleY = y;
	m_ScaleZ = z;
}
