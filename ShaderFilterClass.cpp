#include "ShaderFilterClass.h"

bool ShaderFilterClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = ShaderClass::InitializeShader(device, hwnd, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Filter.ps", false, true);
	if (!result)
		return false;

	bool bufferCreationResult =
		TryCreateBuffer(device, m_bufferDesc, m_CBufferMatrix) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferUtil) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferFilter);
	if (!bufferCreationResult)
		return false;

	return true;
}

bool ShaderFilterClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures)
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

	FilterBufferType* filterPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferFilter.BufferPtr, &filterPtr))
		return false;
	*filterPtr = m_CBufferFilter.BufferValues;
	UnmapFragmentBuffer(deviceContext, 1, &m_CBufferFilter.BufferPtr);

	return true;
}