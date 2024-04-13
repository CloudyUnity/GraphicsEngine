#ifndef _SHADERDEPTH_H_
#define _SHADERDEPTH_H_

#include "ShaderClass.h"

class ShaderDepthClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	CBufferType<MatrixBufferType> m_CBufferMatrix;
};

#endif
