#ifndef _SHADERFIRE_H_
#define _SHADERFIRE_H_

#include "ShaderClass.h"

class ShaderFireClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	CBufferType<MatrixBufferType> m_CBufferMatrix;

	CBufferType<UtilBufferType> m_CBufferUtil;
	CBufferType<FireBufferType> m_CBufferFire;
};

#endif
