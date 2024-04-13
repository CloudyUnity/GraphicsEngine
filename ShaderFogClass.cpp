#include "ShaderFogClass.h"

bool ShaderFogClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = ShaderClass::InitializeShader(device, hwnd, "../GraphicsEngine/Fog.vs", "../GraphicsEngine/Fog.ps", false, false);
	if (!result)
		return false;

	bool bufferCreationResult =
		TryCreateBuffer(device, m_bufferDesc, MatrixBufferType{}) &&
		TryCreateBuffer(device, m_bufferDesc, ClipPlaneBufferType{}) &&
		TryCreateBuffer(device, m_bufferDesc, AlphaBufferType{}) &&
		TryCreateBuffer(device, m_bufferDesc, TexTranslationBufferType{});
	if (!bufferCreationResult)
		return false;

	return true;
}

bool ShaderFogClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures)
{
	bool result;

	result = ShaderClass::SetShaderParameters(deviceContext, textures);
	if (!result)
		return false;

	for (int i = 0; i < m_cbufferList.size(); i++)
	{
		if (!TryMapBuffer(deviceContext, &m_cbufferList.at(i).BufferPtr, &m_cbufferList.at(i).MappedPtr))
			return false;
		*m_cbufferList.at(i).MappedPtr = m_cbufferList.at(i).BufferValues;
		UnmapVertexBuffer(deviceContext, 0, &m_cbufferList.at(i).BufferPtr);
	}

	return true;
}
