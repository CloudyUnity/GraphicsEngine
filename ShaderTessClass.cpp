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

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	result = 
		TryCreateBuffer(device, bufferDesc, m_matrixBuffer, sizeof(MatrixBufferType), m_domainName, "Matrix") &&
		TryCreateBuffer(device, bufferDesc, m_tesselationBuffer, sizeof(TessellationBufferType), m_hullName, "Tess");
	if (!result)
		return false;

	return true;
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

bool ShaderTessClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* texSet, ShaderParameters* params)
{
	bool result;

	result = ShaderClass::SetShaderParameters(deviceContext, texSet, params);
	if (!result)
		return false;

	if (ShaderUsesBuffer(m_domainName, "Matrix"))
	{
		MatrixBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_matrixBuffer, &ptr))
			return false;

		ptr->world = XMMatrixTranspose(params->matrix.world);
		ptr->view = XMMatrixTranspose(params->matrix.view);
		ptr->projection = XMMatrixTranspose(params->matrix.projection);

		UnmapDomainBuffer(deviceContext, 0, &m_matrixBuffer);
	}

	if (ShaderUsesBuffer(m_hullName, "Tess"))
	{
		TessellationBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_tesselationBuffer, &ptr))
			return false;

		ptr->tessellationAmount = params->tesselation.tessellationAmount;

		UnmapHullBuffer(deviceContext, 0, &m_tesselationBuffer);
	}

	if (ShaderUsesBuffer(m_domainName, "Util"))
	{
		UtilBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_utilBuffer, &ptr))
			return false;

		ptr->time = params->utils.time;
		ptr->texelSizeX = params->utils.texelSizeX;
		ptr->texelSizeY = params->utils.texelSizeY;

		UnmapDomainBuffer(deviceContext, 1, &m_utilBuffer);
	}

	return true;
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
