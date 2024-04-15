#ifndef _SHADERTESS_H_
#define _SHADERTESS_H_

#include "ShaderClass.h"

class ShaderTessClass : public ShaderClass
{
public:
	bool Initialize(ID3D11Device*, HWND, char*, char*, char*, char*, bool clampSamplerMode = false);
	void Shutdown() override;

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, WCHAR*, WCHAR*, bool);

	void RenderShader(ID3D11DeviceContext*, int) override;

	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*, ShaderParamsGlobalType*, ShaderParamsObjectType*) override;

	void UnmapHullBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer);
	void UnmapDomainBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer);

private:	
	ID3D11HullShader* m_hullShader;
	ID3D11DomainShader* m_domainShader;	

	std::string m_hullName, m_domainName;
};

#endif
