#ifndef _SHADERREFLECT_H_
#define _SHADERREFLECT_H_

#include "ShaderClass.h"

class ShaderReflectClass : public ShaderClass
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
	CBufferType<ReflectionBufferType> m_CBufferReflect;

	CBufferType<UtilBufferType> m_CBufferUtil;
	CBufferType<LightBufferType> m_CBufferLight;
	CBufferType<LightColorBufferType> m_CBufferLightColor;
	CBufferType<TexTranslationBufferType> m_CBufferTexTrans;
	CBufferType<AlphaBufferType> m_CBufferAlpha;
};

#endif
