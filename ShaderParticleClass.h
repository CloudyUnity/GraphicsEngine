#ifndef _SHADERPARTICLE_H_
#define _SHADERPARTICLE_H_

#include "ShaderClass.h"

class ShaderParticleClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	CBufferType<MatrixBufferType> m_CBufferMatrix;
	CBufferType<CameraBufferType> m_CBufferCamera;
};

#endif
