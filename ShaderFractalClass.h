#ifndef _SHADERFRACTAL_H_
#define _SHADERFRACTAL_H_

#include "ShaderClass.h"

class ShaderFractalClass : public ShaderClass
{
private:
	bool Initialize(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*) override;

public:
	CBufferType<MatrixBufferType> m_CBufferMatrix;
	CBufferType<LightPositionBufferType> m_CBufferLightPos;
	CBufferType<CameraBufferType> m_CBufferCamera;
	CBufferType<FogBufferType> m_CBufferFog;
	CBufferType<ClipPlaneBufferType> m_CBufferClip;

	CBufferType<UtilBufferType> m_CBufferUtil;
	CBufferType<LightBufferType> m_CBufferLight;
	CBufferType<LightColorBufferType> m_CBufferLightColor;
	CBufferType<TexTranslationBufferType> m_CBufferTexTrans;
	CBufferType<AlphaBufferType> m_CBufferAlpha;
};

#endif
