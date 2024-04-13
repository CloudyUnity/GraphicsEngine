#include "ShaderReflectClass.h"

bool ShaderReflectClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = ShaderClass::InitializeShader(device, hwnd, "../GraphicsEngine/Reflect.vs", "../GraphicsEngine/Reflect.ps", false, false);
	if (!result)
		return false;

	bool bufferCreationResult =
		TryCreateBuffer(device, m_bufferDesc, m_CBufferMatrix) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferCamera) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferFog) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferClip) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferAlpha) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferLightColor) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferLight) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferTexTrans) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferUtil) &&
		TryCreateBuffer(device, m_bufferDesc, m_CBufferLightPos);
	if (!bufferCreationResult)
		return false;

	return true;
}

bool ShaderReflectClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures)
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

	FogBufferType* fogPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferFog.BufferPtr, &fogPtr))
		return false;
	*fogPtr = m_CBufferFog.BufferValues;
	UnmapVertexBuffer(deviceContext, 3, &m_CBufferFog.BufferPtr);

	ClipPlaneBufferType* clipPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferClip.BufferPtr, &clipPtr))
		return false;
	*clipPtr = m_CBufferClip.BufferValues;
	UnmapVertexBuffer(deviceContext, 4, &m_CBufferClip.BufferPtr);

	LightPositionBufferType* lightPosPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferLightPos.BufferPtr, &lightPosPtr))
		return false;
	*lightPosPtr = m_CBufferLightPos.BufferValues;
	UnmapVertexBuffer(deviceContext, 1, &m_CBufferLightPos.BufferPtr);

	UtilBufferType* utilPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferUtil.BufferPtr, &utilPtr))
		return false;
	*utilPtr = m_CBufferUtil.BufferValues;
	UnmapFragmentBuffer(deviceContext, 0, &m_CBufferUtil.BufferPtr);

	LightColorBufferType* lightColPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferLightColor.BufferPtr, &lightColPtr))
		return false;
	*lightColPtr = m_CBufferLightColor.BufferValues;
	UnmapFragmentBuffer(deviceContext, 1, &m_CBufferLightColor.BufferPtr);

	LightBufferType* lightPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferLight.BufferPtr, &lightPtr))
		return false;
	*lightPtr = m_CBufferLight.BufferValues;
	UnmapFragmentBuffer(deviceContext, 2, &m_CBufferLight.BufferPtr);

	TexTranslationBufferType* texTransPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferTexTrans.BufferPtr, &texTransPtr))
		return false;
	*texTransPtr = m_CBufferTexTrans.BufferValues;
	UnmapFragmentBuffer(deviceContext, 3, &m_CBufferTexTrans.BufferPtr);

	AlphaBufferType* alphaPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferAlpha.BufferPtr, &alphaPtr))
		return false;
	*alphaPtr = m_CBufferAlpha.BufferValues;
	UnmapFragmentBuffer(deviceContext, 4, &m_CBufferAlpha.BufferPtr);

	ReflectionBufferType* reflectPtr;
	if (!TryMapBuffer(deviceContext, &m_CBufferReflect.BufferPtr, &reflectPtr))
		return false;
	*reflectPtr = m_CBufferReflect.BufferValues;
	UnmapFragmentBuffer(deviceContext, 5, &m_CBufferReflect.BufferPtr);

	return true;
}