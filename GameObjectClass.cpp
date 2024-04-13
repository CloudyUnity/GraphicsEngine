#include "GameObjectClass.h"

GameObjectClass::GameObjectClass()
{
	m_Model = 0;
	m_Shader = 0;
	m_boundingRadius = 0;	
	m_texSetReflectionNum = -1;
	m_texSetRefractionNum = -1;
	m_texSetShadowNum = -1;

	m_BillboardingEnabled = false;
	m_PosX = 0;
	m_PosY = 0;
	m_PosZ = 0;
	m_RotX = 0;
	m_RotY = 0;
	m_RotZ = 0;
	m_ScaleX = 1;
	m_ScaleY = 1;
	m_ScaleZ = 1;

	m_RendTexReflection = 0;
	m_RendTexRefraction = 0;
	m_Textures = 0;
	
	m_reflectMatrix = XMMatrixIdentity();
}

GameObjectClass::~GameObjectClass()
{
}

void GameObjectClass::Initialize(ModelClass* model, ShaderClass* shaders, TextureSetClass* textures, std::string name)
{
	m_Model = model;
	m_Shader = shaders;
	m_Textures = textures;
	m_NameIdentifier = name;
	SetScale(1, 1, 1);
}

bool GameObjectClass::Render(ID3D11DeviceContext* deviceContext, ShaderClass::ShaderParameters* params, ShaderClass* overwriteShader)
{
	XMMATRIX scaleMatrix = XMMatrixScaling(m_ScaleX, m_ScaleY, m_ScaleZ);
	XMMATRIX translateMatrix = XMMatrixTranslation(m_PosX, m_PosY, m_PosZ);	
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(m_RotX * (float)DEG_TO_RAD, m_RotY * (float)DEG_TO_RAD, m_RotZ * (float)DEG_TO_RAD);

	if (m_BillboardingEnabled)
	{
		float angle = atan2(m_PosX - params->camera.cameraPosition.x, m_PosZ - params->camera.cameraPosition.z);
		rotateMatrix = XMMatrixMultiply(rotateMatrix, XMMatrixRotationY(angle));
	}

	XMMATRIX srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
	params->matrix.world = XMMatrixMultiply(srMatrix, translateMatrix);

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->RenderBuffers(deviceContext);

	if (overwriteShader)
	{
		return overwriteShader->Render(deviceContext, m_Model->GetIndexCount(), nullptr, params);
	}

	bool result = m_Shader->Render(deviceContext, m_Model->GetIndexCount(), m_Textures, params);
	if (!result)
		return false;

	return true;
}

void GameObjectClass::Shutdown() 
{
	if (m_Model) {
		m_Model = 0;
	}

	if (m_Shader) {
		m_Shader = 0;
	}

	if (m_Textures)	{
		m_Textures = 0;
	}

	if (m_RendTexReflection)
	{
		m_RendTexReflection->Shutdown();
		delete m_RendTexReflection;
		m_RendTexReflection = 0;
	}

	if (m_RendTexRefraction)
	{
		m_RendTexRefraction->Shutdown();
		delete m_RendTexRefraction;
		m_RendTexRefraction = 0;
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

	m_boundingRadius = m_Model->GetBoundingRadius() * max(max(m_ScaleX, m_ScaleY), m_ScaleZ);
}

void GameObjectClass::SetScale(float x)
{
	m_ScaleX = x;
	m_ScaleY = x;
	m_ScaleZ = x;

	m_boundingRadius = m_Model->GetBoundingRadius() * max(max(m_ScaleX, m_ScaleY), m_ScaleZ);
}

float GameObjectClass::GetBoundingRadius() 
{
	return m_boundingRadius;
}

void GameObjectClass::SubscribeToReflection(ID3D11Device* device, int texSetNum, int format)
{
	m_RendTexReflection = new RenderTextureClass;
	m_RendTexReflection->Initialize(device, SCREEN_X, SCREEN_Y, SCREEN_DEPTH, SCREEN_NEAR, format);

	m_texSetReflectionNum = texSetNum;
}

void GameObjectClass::SubscribeToRefraction(ID3D11Device* device, int texSetNum, int format)
{
	m_RendTexRefraction = new RenderTextureClass;
	m_RendTexRefraction->Initialize(device, SCREEN_X, SCREEN_Y, SCREEN_DEPTH, SCREEN_NEAR, format);

	m_texSetRefractionNum = texSetNum;
}

void GameObjectClass::SubscribeToShadows(int texSetNum)
{	
	m_texSetShadowNum = texSetNum;
}

void GameObjectClass::SetReflectionTex()
{
	m_Textures->Add(m_RendTexReflection->GetShaderResourceView(), m_texSetReflectionNum);
}

void GameObjectClass::SetRefractionTex()
{
	m_Textures->Add(m_RendTexRefraction->GetShaderResourceView(), m_texSetRefractionNum);
}

void GameObjectClass::SetShadowTex(RenderTextureClass* rendTex)
{
	m_Textures->Add(rendTex->GetShaderResourceView(), m_texSetShadowNum);
}

void GameObjectClass::SetReflectionMatrix(XMMATRIX matrix)
{
	m_reflectMatrix = matrix;
}

XMMATRIX GameObjectClass::GetReflectionMatrix()
{
	return m_reflectMatrix;
}

bool GameObjectClass::IsSubscribedToShadows()
{
	return m_texSetShadowNum != -1;
}

void GameObjectClass::SetBillBoarding(bool billboarding)
{
	m_BillboardingEnabled = billboarding;
}

void GameObjectClass::SetBackCulling(bool enabled)
{
	m_BackCullingDisabled = !enabled;
}
