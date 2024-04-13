#ifndef _SHADER2D_H_
#define _SHADER2D_H_

#include "ShaderClass.h"

class Shader2DClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	CBufferType<MatrixBufferType> m_CBufferMatrix;

	CBufferType<PixelBufferType> m_CBufferPixel;
};

#endif
