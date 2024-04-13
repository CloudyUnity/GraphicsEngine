#include "ShaderFontClass.h"

bool ShaderFontClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = ShaderClass::InitializeShader(device, hwnd, "../GraphicsEngine/Simple.vs", "../GraphicsEngine/Font.ps", false, true);
	if (!result)
		return false;

	bool bufferCreationResult =
		TryCreateBuffer(device, m_bufferDesc, m_CBufferMatrix) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferPixel);
	if (!bufferCreationResult)
		return false;

	return true;
}

bool ShaderFontClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures)
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

	PixelBufferType* pixelPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferPixel.BufferPtr, &pixelPtr))
		return false;
	*pixelPtr = m_CBufferPixel.BufferValues;
	UnmapVertexBuffer(deviceContext, 0, &m_CBufferPixel.BufferPtr);

	return true;
}