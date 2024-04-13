#include "ShaderSkyboxClass.h"

bool ShaderSkyboxClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = ShaderClass::InitializeShader(device, hwnd, "../GraphicsEngine/Skybox.vs", "../GraphicsEngine/Skybox.ps", true, true);
	if (!result)
		return false;

	bool bufferCreationResult =
		TryCreateBuffer(device, m_bufferDesc, m_CBufferMatrix);
	if (!bufferCreationResult)
		return false;

	return true;
}

bool ShaderSkyboxClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures)
{
	bool result;

	result = ShaderClass::SetShaderParameters(deviceContext, textures);
	if (!result)
		return false;

	MatrixBufferType* matrixPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferMatrix.BufferPtr, &matrixPtr))
		return false;
	*matrixPtr = m_CBufferMatrix.BufferValues;
	UnmapVertexBuffer(deviceContext, 0, &m_CBufferMatrix.BufferPtr);

	return true;
}