#include "ShaderFireClass.h"

bool ShaderFireClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = ShaderClass::InitializeShader(device, hwnd, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Fire.ps", true, true);
	if (!result)
		return false;

	bool bufferCreationResult =
		TryCreateBuffer(device, m_bufferDesc, m_CBufferMatrix) &&	
		TryCreateBuffer(device, m_bufferDesc, m_CBufferUtil) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferFire);
	if (!bufferCreationResult)
		return false;

	return true;
}

bool ShaderFireClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures)
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

	UtilBufferType* utilPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferUtil.BufferPtr, &utilPtr))
		return false;
	*utilPtr = m_CBufferUtil.BufferValues;
	UnmapFragmentBuffer(deviceContext, 0, &m_CBufferUtil.BufferPtr);

	FireBufferType* firePtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferFire.BufferPtr, &firePtr))
		return false;
	*firePtr = m_CBufferFire.BufferValues;
	UnmapFragmentBuffer(deviceContext, 1, &m_CBufferFire.BufferPtr);
	
	return true;
}