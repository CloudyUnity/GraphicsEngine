#ifndef _SHADERGENERIC_H_
#define _SHADERGENERIC_H_

#include "ShaderClass.h"

template<typename... Types>
class CBufferContainer;

template<>
class CBufferContainer<> { };

template<typename T, typename... Rest>
class CBufferContainer<T, Rest...> : public CBufferContainer<Rest...>
{
public:
	CBufferContainer(const T& cbuffer) : cbuffer(cbuffer) {}
	T cbuffer;
};

template<typename T, typename... Rest>
class ShaderGenericClass : public ShaderClass
{
private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, bool) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;
private:
	CBufferContainer m_CBuffers;
};

#endif
