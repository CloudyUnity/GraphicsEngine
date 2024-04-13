#ifndef _SHADERSKYBOX_H_
#define _SHADERSKYBOX_H_

#include "ShaderClass.h"

class ShaderSkyboxClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	CBufferType<MatrixBufferType> m_CBufferMatrix;
};

#endif
