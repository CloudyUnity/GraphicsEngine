#ifndef _SHADERFILTER_H_
#define _SHADERFILTER_H_

#include "ShaderClass.h"

class ShaderFilterClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	CBufferType<MatrixBufferType> m_CBufferMatrix;

	CBufferType<UtilBufferType> m_CBufferUtil;
	CBufferType<FilterBufferType> m_CBufferFilter;
};

#endif
