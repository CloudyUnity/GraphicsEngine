#include "ShaderTessClass.h"

bool ShaderTessClass::Initialize(ID3D11Device* device, HWND hwnd, char* vsPath, char* hsPath, char* dsPath, char* psPath, bool clampSamplerMode)
{
	wchar_t vsFilename[128];
	wchar_t hsFilename[128];
	wchar_t dsFilename[128];
	wchar_t psFilename[128];
	int error;

	char* lastSlash = strrchr(vsPath, '/');
	m_vertexName = lastSlash != nullptr ? lastSlash + 1 : vsPath;
	lastSlash = strrchr(hsPath, '/');
	m_hullName = lastSlash != nullptr ? lastSlash + 1 : hsPath;
	lastSlash = strrchr(dsPath, '/');
	m_domainName = lastSlash != nullptr ? lastSlash + 1 : dsPath;
	lastSlash = strrchr(psPath, '/');
	m_fragName = lastSlash != nullptr ? lastSlash + 1 : psPath;

	size_t requiredSize;
	mbstowcs_s(&requiredSize, nullptr, 0, vsPath, 0);
	if (requiredSize == -1)
		return false;

	error = mbstowcs_s(nullptr, vsFilename, requiredSize, vsPath, requiredSize);
	if (error != 0)
		return false;

	mbstowcs_s(&requiredSize, nullptr, 0, hsPath, 0);
	if (requiredSize == -1)
		return false;

	error = mbstowcs_s(nullptr, hsFilename, requiredSize, hsPath, requiredSize);
	if (error != 0)
		return false;

	mbstowcs_s(&requiredSize, nullptr, 0, dsPath, 0);
	if (requiredSize == -1)
		return false;

	error = mbstowcs_s(nullptr, dsFilename, requiredSize, dsPath, requiredSize);
	if (error != 0)
		return false;

	mbstowcs_s(&requiredSize, nullptr, 0, psPath, 0);
	if (requiredSize == -1)
		return false;

	error = mbstowcs_s(nullptr, psFilename, requiredSize, psPath, requiredSize);
	if (error != 0)
		return false;

	return InitializeShader(device, hwnd, vsFilename, hsFilename, dsFilename, psFilename, clampSamplerMode);
}

void ShaderTessClass::Shutdown()
{
	if (m_hullShader)
	{
		m_hullShader->Release();
		m_hullShader = 0;
	}

	if (m_domainShader)
	{
		m_domainShader->Release();
		m_domainShader = 0;
	}
}

bool ShaderTessClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsName, WCHAR* hsName, WCHAR* dsName, WCHAR* psName, bool clamp)
{
	bool result;
	HRESULT hResult;

	result = ShaderClass::InitializeShader(device, hwnd, vsName, psName, clamp);
	if (!result)
		return false;

	ID3D10Blob* hullShaderBuffer;
	ID3D10Blob* domainShaderBuffer;
	ID3D10Blob* errorMessage = 0;

	hResult = D3DCompileFromFile(hsName, NULL, NULL, "HS_MAIN", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&hullShaderBuffer, &errorMessage);
	if (FAILED(hResult))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hsName);
			return false;
		}

		MessageBox(hwnd, hsName, L"Missing Shader File", MB_OK);
		return false;
	}

	hResult = D3DCompileFromFile(dsName, NULL, NULL, "DS_MAIN", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&domainShaderBuffer, &errorMessage);
	if (FAILED(hResult))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, dsName);
			return false;
		}

		MessageBox(hwnd, dsName, L"Missing Shader File", MB_OK);
		return false;
	}

	hResult = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(hResult))
		return false;

	hResult = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(hResult))
		return false;

	result = SaveCBufferInfo(hullShaderBuffer, domainShaderBuffer);
	if (!result)
		return false;

	hullShaderBuffer->Release();
	hullShaderBuffer = 0;

	domainShaderBuffer->Release();
	domainShaderBuffer = 0;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	result = 
		TryCreateBuffer(device, bufferDesc, m_matrixBuffer, sizeof(MatrixBufferType), "MatrixBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_fogBuffer, sizeof(FogBufferType), "FogBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_clipBuffer, sizeof(ClipPlaneBufferType), "ClipBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_cameraBuffer, sizeof(CameraBufferType), "CameraBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_reflectionBuffer, sizeof(ReflectionBufferType), "ReflectionBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_utilBuffer, sizeof(UtilBufferType), "UtilBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_lightColorBuffer, sizeof(LightColorBufferType), "LightColorBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_lightBuffer, sizeof(LightBufferType), "LightBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_pixelBuffer, sizeof(PixelBufferType), "PixelBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_texTransBuffer, sizeof(TexTranslationBufferType), "TextureTranslationBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_alphaBuffer, sizeof(AlphaBufferType), "AlphaBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_waterBuffer, sizeof(WaterBufferType), "WaterBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_fireBuffer, sizeof(FireBufferType), "FireBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_shadowBuffer, sizeof(ShadowBufferType), "ShadowBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_blurBuffer, sizeof(BlurBufferType), "BlurBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_filterBuffer, sizeof(FilterBufferType), "FilterBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_lightPositionBuffer, sizeof(LightPositionBufferType), "LightPositionBuffer") &&

		TryCreateBuffer(device, bufferDesc, m_oceanSineBuffer, sizeof(OceanSineBufferType), "OceanSineBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_tesselationBuffer, sizeof(TessellationBufferType), "TessellationBuffer");
	if (!result)
		return false;

	return true;
}

bool ShaderTessClass::SaveCBufferInfo(ID3D10Blob* hullBlob, ID3D10Blob* domainBlob)
{
	HRESULT result;

	ID3D11ShaderReflection* hReflection = nullptr, * dReflection = nullptr;

	result = D3DReflect(hullBlob->GetBufferPointer(), hullBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&hReflection);
	if (FAILED(result))
		return false;

	result = D3DReflect(domainBlob->GetBufferPointer(), domainBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&dReflection);
	if (FAILED(result))
		return false;

	D3D11_SHADER_DESC hDesc, dDesc;
	hReflection->GetDesc(&hDesc);
	dReflection->GetDesc(&dDesc);

	for (UINT i = 0; i < hDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* vBuffer = hReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		vBuffer->GetDesc(&bufferDesc);
		m_cbufferListHull.push_back(bufferDesc.Name);
	}

	for (UINT i = 0; i < dDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pBuffer = dReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		pBuffer->GetDesc(&bufferDesc);
		m_cbufferListDomain.push_back(bufferDesc.Name);
	}

	if (dReflection)
		dReflection->Release();

	if (hReflection)
		hReflection->Release();

	return true;
}

bool ShaderTessClass::UsesCBuffer(string cbufferName)
{
	return ShaderClass::UsesCBuffer(cbufferName) || UsesCBufferHull(cbufferName) != -1 || UsesCBufferDomain(cbufferName) != -1;
}

int ShaderTessClass::UsesCBufferHull(string cbufferName)
{
	auto it = std::find(m_cbufferListHull.begin(), m_cbufferListHull.end(), cbufferName);
	if (it == m_cbufferListHull.end())
		return -1;

	return it - m_cbufferListHull.begin();
}

int ShaderTessClass::UsesCBufferDomain(string cbufferName)
{
	auto it = std::find(m_cbufferListDomain.begin(), m_cbufferListDomain.end(), cbufferName);
	if (it == m_cbufferListDomain.end())
		return -1;

	return it - m_cbufferListDomain.begin();
}

void ShaderTessClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	deviceContext->DrawIndexed(indexCount, 0, 0);

	deviceContext->HSSetShader(NULL, NULL, 0);
	deviceContext->DSSetShader(NULL, NULL, 0);
}

bool ShaderTessClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* texSet, ShaderParamsGlobalType* globalParams, ShaderParamsObjectType* objectParams)
{
	bool result;

	result = ShaderClass::SetShaderParameters(deviceContext, texSet, globalParams, objectParams);
	if (!result)
		return false;

	int bufferIndex;

	bufferIndex = UsesCBufferDomain("MatrixBuffer");
	if (bufferIndex != -1)
	{
		MatrixBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_matrixBuffer, &ptr))
			return false;
		*ptr = objectParams->matrix;
		UnmapDomainBuffer(deviceContext, bufferIndex, &m_matrixBuffer);
	}

	bufferIndex = UsesCBufferHull("TessellationBuffer");
	if (bufferIndex != -1)
	{
		TessellationBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_tesselationBuffer, &ptr))
			return false;
		*ptr = objectParams->tesselation;
		UnmapHullBuffer(deviceContext, bufferIndex, &m_tesselationBuffer);
	}

	bufferIndex = UsesCBufferDomain("UtilBuffer");
	if (bufferIndex != -1)
	{
		UtilBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_utilBuffer, &ptr))
			return false;
		*ptr = globalParams->utils;
		UnmapDomainBuffer(deviceContext, bufferIndex, &m_utilBuffer);
	}

	bufferIndex = UsesCBufferDomain("OceanSineBuffer");
	if (bufferIndex != -1)
	{
		OceanSineBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_oceanSineBuffer, &ptr))
			return false;
		*ptr = globalParams->oceanSine;
		UnmapDomainBuffer(deviceContext, bufferIndex, &m_oceanSineBuffer);
	}

	bufferIndex = UsesCBufferDomain("CameraBuffer");
	if (bufferIndex != -1)
	{
		CameraBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_cameraBuffer, &ptr))
			return false;
		*ptr = globalParams->camera;
		UnmapDomainBuffer(deviceContext, bufferIndex, &m_cameraBuffer);
	}

	return true;
}

bool ShaderTessClass::TryCreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer*& ptr, size_t structSize, string bufferName)
{
	if (!UsesCBuffer(bufferName) || ptr != nullptr)
		return true;

	bufferDesc.ByteWidth = (UINT)structSize;
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &ptr);

	m_bufferList.push_back(ptr);

	return !FAILED(result);
}

void ShaderTessClass::UnmapHullBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer)
{
	deviceContext->Unmap(*buffer, 0);
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, buffer);
}

void ShaderTessClass::UnmapDomainBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer)
{
	deviceContext->Unmap(*buffer, 0);
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, buffer);
}
