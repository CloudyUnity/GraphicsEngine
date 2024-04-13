#include "ShaderDepthClass.h"

bool ShaderDepthClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = ShaderClass::InitializeShader(device, hwnd, "../GraphicsEngine/Depth.vs", "../GraphicsEngine/Depth.ps", false, false);
	if (!result)
		return false;

	bool bufferCreationResult =
		TryCreateBuffer(device, m_bufferDesc, m_CBufferMatrix);
	if (!bufferCreationResult)
		return false;

	return true;
}

bool ShaderDepthClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures)
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