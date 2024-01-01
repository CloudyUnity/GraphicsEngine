#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_utilBuffer = 0;
	m_sampleState = 0;
	m_lightColorBuffer = 0;
	m_lightPositionBuffer = 0;
	m_lightBuffer = 0;
	m_cameraBuffer = 0;
	m_pixelBuffer = 0;	
	m_fogBuffer = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}


TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd, char* vertexName, char* fragName)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	char* lastSlash = strrchr(vertexName, '/');
	m_vertexName = lastSlash != nullptr ? lastSlash + 1 : vertexName;
	lastSlash = strrchr(fragName, '/');
	m_fragName = lastSlash != nullptr ? lastSlash + 1 : fragName;

	size_t requiredSize;
	mbstowcs_s(&requiredSize, nullptr, 0, vertexName, 0);
	if (requiredSize == -1)
		return false;		

	error = mbstowcs_s(nullptr, vsFilename, requiredSize, vertexName, requiredSize);
	if (error != 0)
		return false;		

	mbstowcs_s(&requiredSize, nullptr, 0, fragName, 0);
	if (requiredSize == -1)
		return false;

	error = mbstowcs_s(nullptr, psFilename, requiredSize, fragName, requiredSize);
	if (error != 0)
		return false;

	return InitializeShader(device, hwnd, vsFilename, psFilename);
}

void TextureShaderClass::Shutdown()
{
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	TextureClass* textures, int texCount, unordered_map<string, any> arguments)
{
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textures, texCount, arguments);
	if (!result)
		return false;

	RenderShader(deviceContext, indexCount);
	return true;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
		return false;

	RenderShader(deviceContext, indexCount);
	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "VS_MAIN", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
			return false;
		}

		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "PS_MAIN", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
			return false;
		}

		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;	
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	if (ShaderUsesBuffer(m_vertexName, "Matrix")) 
	{
		bufferDesc.ByteWidth = sizeof(MatrixBufferType);
		result = device->CreateBuffer(&bufferDesc, NULL, &m_matrixBuffer);
		if (FAILED(result))
			return false;
	}

	if (ShaderUsesBuffer(m_vertexName, "Fog"))
	{
		bufferDesc.ByteWidth = sizeof(FogBufferType);
		result = device->CreateBuffer(&bufferDesc, NULL, &m_fogBuffer);
		if (FAILED(result))
			return false;
	}

	if (ShaderUsesBuffer(m_fragName, "Util"))
	{
		bufferDesc.ByteWidth = sizeof(UtilBufferType);
		result = device->CreateBuffer(&bufferDesc, NULL, &m_utilBuffer);
		if (FAILED(result))
			return false;
	}

	if (ShaderUsesBuffer(m_fragName, "LightColor"))
	{
		bufferDesc.ByteWidth = sizeof(LightColorBufferType);
		result = device->CreateBuffer(&bufferDesc, NULL, &m_lightColorBuffer);
		if (FAILED(result))
			return false;
	}

	if (ShaderUsesBuffer(m_fragName, "LightPosition"))
	{
		bufferDesc.ByteWidth = sizeof(LightPositionBufferType);
		result = device->CreateBuffer(&bufferDesc, NULL, &m_lightPositionBuffer);
		if (FAILED(result))
			return false;
	}

	if (ShaderUsesBuffer(m_fragName, "Light"))
	{
		bufferDesc.ByteWidth = sizeof(LightBufferType);
		result = device->CreateBuffer(&bufferDesc, NULL, &m_lightBuffer);
		if (FAILED(result))
			return false;
	}

	if (ShaderUsesBuffer(m_fragName, "Camera"))
	{
		bufferDesc.ByteWidth = sizeof(CameraBufferType);
		result = device->CreateBuffer(&bufferDesc, NULL, &m_cameraBuffer);
		if (FAILED(result))
			return false;
	}

	if (ShaderUsesBuffer(m_fragName, "Pixel"))
	{
		bufferDesc.ByteWidth = sizeof(PixelBufferType);
		result = device->CreateBuffer(&bufferDesc, NULL, &m_pixelBuffer);
		if (FAILED(result))
			return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
		return false;

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_utilBuffer)
	{
		m_utilBuffer->Release();
		m_utilBuffer = 0;
	}

	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}

	if (m_lightColorBuffer)
	{
		m_lightColorBuffer->Release();
		m_lightColorBuffer = 0;
	}

	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if (m_lightPositionBuffer)
	{
		m_lightPositionBuffer->Release();
		m_lightPositionBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_fogBuffer) 
	{
		m_fogBuffer->Release();
		m_fogBuffer = 0;
	}
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	TextureClass* textures, int texCount, unordered_map<string, any> arguments)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	for (int i = 0; i < texCount; i++)
	{
		ID3D11ShaderResourceView* tex = textures[i].GetTexture();
		deviceContext->PSSetShaderResources(i, 1, &tex);
	}

	if (ShaderUsesBuffer(m_vertexName, "Matrix"))
	{
		result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto matrixPtr = (MatrixBufferType*)mappedResource.pData;
		matrixPtr->world = worldMatrix;
		matrixPtr->view = viewMatrix;
		matrixPtr->projection = projectionMatrix;
		deviceContext->Unmap(m_matrixBuffer, 0);
		bufferNumber = 0;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	}

	if (ShaderUsesBuffer(m_vertexName, "Fog"))
	{
		result = deviceContext->Map(m_fogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto fogPtr = (FogBufferType*)mappedResource.pData;
		fogPtr->fogStart = any_cast<float>(arguments.at("FogStart"));
		fogPtr->fogEnd = any_cast<float>(arguments.at("FogEnd"));
		deviceContext->Unmap(m_fogBuffer, 0);
		bufferNumber = 3;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_fogBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "Util"))
	{
		result = deviceContext->Map(m_utilBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto utilPtr = (UtilBufferType*)mappedResource.pData;
		utilPtr->time = any_cast<float>(arguments.at("Time"));
		deviceContext->Unmap(m_utilBuffer, 0);
		bufferNumber = 0;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_utilBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "Camera"))
	{
		result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto camPtr = (CameraBufferType*)mappedResource.pData;
		camPtr->cameraPosition = any_cast<XMFLOAT3>(arguments.at("CameraPos"));
		deviceContext->Unmap(m_cameraBuffer, 0);
		bufferNumber = 2;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "LightPosition"))
	{
		XMFLOAT4* lightPosition = any_cast<XMFLOAT4*>(arguments.at("LightPosition"));

		result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto lightPosPtr = (LightPositionBufferType*)mappedResource.pData;
		lightPosPtr->lightPosition[0] = lightPosition[0];
		lightPosPtr->lightPosition[1] = lightPosition[1];
		lightPosPtr->lightPosition[2] = lightPosition[2];
		lightPosPtr->lightPosition[3] = lightPosition[3];
		deviceContext->Unmap(m_lightPositionBuffer, 0);
		bufferNumber = 1;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightPositionBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "LightColor"))
	{
		XMFLOAT4* diffuseColor = any_cast<XMFLOAT4*>(arguments.at("DiffuseColor"));

		result = deviceContext->Map(m_lightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto lightColPtr = (LightColorBufferType*)mappedResource.pData;
		lightColPtr->diffuseColor[0] = diffuseColor[0];
		lightColPtr->diffuseColor[1] = diffuseColor[1];
		lightColPtr->diffuseColor[2] = diffuseColor[2];
		lightColPtr->diffuseColor[3] = diffuseColor[3];
		deviceContext->Unmap(m_lightColorBuffer, 0);
		bufferNumber = 1;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightColorBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "Light"))
	{
		LightClass* dirLight = any_cast<LightClass*>(arguments.at("DirLight"));

		result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto lightPtr = (LightBufferType*)mappedResource.pData;
		lightPtr->ambientColor = dirLight->GetAmbientColor();
		lightPtr->diffuseColor = dirLight->GetDiffuseColor();
		lightPtr->lightDirection = dirLight->GetDirection();
		lightPtr->specularColor = dirLight->GetSpecularColor();
		lightPtr->specularPower = dirLight->GetSpecularPower();
		deviceContext->Unmap(m_lightBuffer, 0);
		bufferNumber = 2;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "Pixel"))
	{
		result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto pixelPtr = (PixelBufferType*)mappedResource.pData;
		pixelPtr->pixelColor = any_cast<XMFLOAT4>(arguments.at("Pixel"));
		deviceContext->Unmap(m_pixelBuffer, 0);
		bufferNumber = 0;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);
	}

	return true;
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matrixPtr;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;
	matrixPtr = (MatrixBufferType*)mappedResource.pData;
	matrixPtr->world = worldMatrix;
	matrixPtr->view = viewMatrix;
	matrixPtr->projection = projectionMatrix;
	deviceContext->Unmap(m_matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

bool TextureShaderClass::ShaderUsesBuffer(std::string shader, std::string buffer)
{
	if (shader == "Light.vs") {
		return buffer == "Matrix" ||
			buffer == "Camera";
	}

	if (shader == "Fog.vs") {
		return buffer == "Matrix" ||
			buffer == "Camera" ||
			buffer == "Fog";
	}

	if (shader == "MultiTex.ps" || shader == "Fog.ps") {
		return buffer == "LightColor" ||
			buffer == "Util" ||
			buffer == "Light" ||
			buffer == "LightPosition";
	}

	if (shader == "Font.ps") {
		return buffer == "Pixel";
	}

	if (shader == "2D.ps") {
		return buffer == "Util";
	}

	return false;
}