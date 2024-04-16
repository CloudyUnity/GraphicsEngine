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

	bool SaveCBufferInfo(ID3D10Blob*, ID3D10Blob*);
	bool UsesCBuffer(string) override;
	int UsesCBufferHull(string);
	int UsesCBufferDomain(string);

	void RenderShader(ID3D11DeviceContext*, int) override;

	bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*, ShaderParamsGlobalType*, ShaderParamsObjectType*) override;

	bool TryCreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer*& ptr, size_t structSize, string);
	void UnmapHullBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer);
	void UnmapDomainBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer);

private:	
	ID3D11HullShader* m_hullShader;
	ID3D11DomainShader* m_domainShader;	

	string m_hullName, m_domainName;
	vector<string> m_cbufferListHull;
	vector<string> m_cbufferListDomain;

	template<typename T>
	bool SetShaderCBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, T values, string name)
	{
		int bufferIndex;

		bufferIndex = UsesCBufferHull(name);
		if (bufferIndex != -1)
		{
			T* ptr;
			if (!TryMapBuffer(deviceContext, &buffer, &ptr))
				return false;
			*ptr = values;
			UnmapHullBuffer(deviceContext, bufferIndex, &buffer);
		}

		bufferIndex = UsesCBufferDomain(name);
		if (bufferIndex != -1)
		{
			T* ptr;
			if (!TryMapBuffer(deviceContext, &buffer, &ptr))
				return false;
			*ptr = values;
			UnmapDomainBuffer(deviceContext, bufferIndex, &buffer);
		}
	}
};

#endif
