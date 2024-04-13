#ifndef _SHADERFONT_H_
#define _SHADERFONT_H_

#include "ShaderClass.h"

class ShaderFontClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	CBufferType<MatrixBufferType> m_CBufferMatrix;

	CBufferType<PixelBufferType> m_CBufferPixel;
};

#endif
