#include "ShaderParticleClass.h"

bool ShaderParticleClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = ShaderClass::InitializeShader(device, hwnd, "../GraphicsEngine/Particle.vs", "../GraphicsEngine/Particle.ps", false, false);
	if (!result)
		return false;

	bool bufferCreationResult =
		TryCreateBuffer(device, m_bufferDesc, m_CBufferMatrix) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferCamera);
	if (!bufferCreationResult)
		return false;

	return true;
}

bool ShaderParticleClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures)
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

	CameraBufferType* camPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferCamera.BufferPtr, &camPtr))
		return false;
	*camPtr = m_CBufferCamera.BufferValues;
	UnmapVertexBuffer(deviceContext, 2, &m_CBufferCamera.BufferPtr);

	return true;
}