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

	for (auto hBuffer : m_cbufferListHull)
	{
		result = TryCreateBufferHull(device, bufferDesc, hBuffer);
		if (!result)
			return false;
	}

	for (auto dBuffer : m_cbufferListDomain)
	{
		result = TryCreateBufferDomain(device, bufferDesc, dBuffer);
		if (!result)
			return false;
	}

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
		m_cbufferSizeListHull.push_back(bufferDesc.Size);
	}

	for (UINT i = 0; i < dDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pBuffer = dReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		pBuffer->GetDesc(&bufferDesc);
		m_cbufferListDomain.push_back(bufferDesc.Name);
		m_cbufferSizeListDomain.push_back(bufferDesc.Size);
	}

	if (dReflection)
		dReflection->Release();

	if (hReflection)
		hReflection->Release();

	return true;
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

	result =
		SetShaderCBuffer(deviceContext, globalParams->camera, "CameraBuffer") &&
		SetShaderCBuffer(deviceContext, globalParams->fog, "FogBuffer") &&
		SetShaderCBuffer(deviceContext, globalParams->utils, "UtilBuffer") &&
		SetShaderCBuffer(deviceContext, globalParams->lightPos, "LightPositionBuffer") &&
		SetShaderCBuffer(deviceContext, globalParams->lightColor, "LightColorBuffer") &&
		SetShaderCBuffer(deviceContext, globalParams->light, "LightBuffer") &&
		SetShaderCBuffer(deviceContext, globalParams->shadow, "ShadowBuffer") &&

		SetShaderCBuffer(deviceContext, objectParams->matrix, "MatrixBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->clip, "ClipPlaneBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->pixel, "PixelBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->textureTranslation, "TextureTranslationBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->alpha, "AlphaBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->water, "WaterBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->fire, "FireBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->blur, "BlurBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->filter, "FilterBuffer") &&

		// HS and DS specific Cbuffers
		SetShaderCBuffer(deviceContext, globalParams->oceanSine, "OceanSineBuffer") &&
		SetShaderCBuffer(deviceContext, objectParams->tesselation, "TessellationBuffer");
	if (!result)
		return false;


	if (globalParams->reflectionEnabled)
	{
		result = SetShaderCBuffer(deviceContext, globalParams->reflection, "ReflectionBuffer");
		if (!result)
			return false;
	}

	return true;
}

bool ShaderTessClass::TryCreateBufferHull(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, string bufferName)
{
	int bufferIndex = UsesCBufferHull(bufferName);
	if (bufferIndex == -1)
		return false;

	ID3D11Buffer* ptr;
	bufferDesc.ByteWidth = (UINT)m_cbufferSizeListHull.at(bufferIndex);
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &ptr);

	m_bufferPtrListHull.push_back(ptr);

	return !FAILED(result);
}

bool ShaderTessClass::TryCreateBufferDomain(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, string bufferName)
{
	int bufferIndex = UsesCBufferDomain(bufferName);
	if (bufferIndex == -1)
		return false;

	ID3D11Buffer* ptr;
	bufferDesc.ByteWidth = (UINT)m_cbufferSizeListDomain.at(bufferIndex);
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &ptr);

	m_bufferPtrListDomain.push_back(ptr);

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
