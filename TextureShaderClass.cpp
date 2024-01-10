#include "textureshaderclass.h"

ShaderClass::ShaderClass()
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
	m_clipBuffer = 0;
	m_texTransBuffer = 0;
	m_reflectionBuffer = 0;
	m_waterBuffer = 0;
}

ShaderClass::ShaderClass(const ShaderClass& other)
{
}


ShaderClass::~ShaderClass()
{
}

bool ShaderClass::Initialize(ID3D11Device* device, HWND hwnd, char* vertexName, char* fragName)
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

void ShaderClass::Shutdown()
{
	ShutdownShader();
}

bool ShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, TextureSetClass* textures, ShaderParameters* parameters)
{
	bool result = SetShaderParameters(deviceContext, textures, parameters);
	if (!result)
		return false;

	RenderShader(deviceContext, indexCount);
	return true;
}

bool ShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
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

	bool bufferCreationResult = 
		TryCreateBuffer(device, bufferDesc, &m_matrixBuffer, sizeof(MatrixBufferType), m_vertexName, "Matrix") &&
		TryCreateBuffer(device, bufferDesc, &m_fogBuffer, sizeof(FogBufferType), m_vertexName, "Fog") &&
		TryCreateBuffer(device, bufferDesc, &m_clipBuffer, sizeof(ClipPlaneBufferType), m_vertexName, "Clip") &&
		TryCreateBuffer(device, bufferDesc, &m_cameraBuffer, sizeof(CameraBufferType), m_vertexName, "Camera") &&
		TryCreateBuffer(device, bufferDesc, &m_reflectionBuffer, sizeof(ReflectionBufferType), m_vertexName, "Reflection") &&
		TryCreateBuffer(device, bufferDesc, &m_utilBuffer, sizeof(UtilBufferType), m_fragName, "Util") &&
		TryCreateBuffer(device, bufferDesc, &m_lightColorBuffer, sizeof(LightColorBufferType), m_fragName, "LightColor") &&
		TryCreateBuffer(device, bufferDesc, &m_lightBuffer, sizeof(LightBufferType), m_fragName, "Light") &&
		TryCreateBuffer(device, bufferDesc, &m_pixelBuffer, sizeof(PixelBufferType), m_fragName, "Pixel") &&
		TryCreateBuffer(device, bufferDesc, &m_texTransBuffer, sizeof(TexTranslationBufferType), m_fragName, "TexTranslation") &&
		TryCreateBuffer(device, bufferDesc, &m_alphaBuffer, sizeof(AlphaBufferType), m_fragName, "Alpha") &&		
		TryCreateBuffer(device, bufferDesc, &m_waterBuffer, sizeof(WaterBufferType), m_fragName, "Water") &&		
		TryCreateBuffer(device, bufferDesc, &m_lightPositionBuffer, sizeof(LightPositionBufferType), m_fragName, "LightPosition");

	if (!bufferCreationResult)
		return false;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	if (SAMPLER_MODE == 1)
	{
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	}
	else if (SAMPLER_MODE == 0)
	{
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	}	

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

bool ShaderClass::TryCreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer** ptr, size_t structSize, string shaderName, string bufferName)
{
	if (!ShaderUsesBuffer(shaderName, bufferName))
		return true;

	m_bufferList.push_back(*ptr);

	bufferDesc.ByteWidth = structSize;
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, ptr);
	return !FAILED(result);
}

void ShaderClass::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	for (auto buffer : m_bufferList)
	{
		if (buffer)
			buffer->Release();
	}
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

bool ShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures, ShaderParameters* params)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	for (int i = 0; i < textures->GetCount(); i++)
	{
		ID3D11ShaderResourceView* tex = textures->GetTexture(i);
		deviceContext->PSSetShaderResources(i, 1, &tex);
	}

	if (ShaderUsesBuffer(m_vertexName, "Matrix"))
	{
		result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto matrixPtr = (MatrixBufferType*)mappedResource.pData;
		matrixPtr->world = XMMatrixTranspose(params->matrix.world);
		matrixPtr->view = XMMatrixTranspose(params->matrix.view);
		matrixPtr->projection = XMMatrixTranspose(params->matrix.projection);
		deviceContext->Unmap(m_matrixBuffer, 0);
		bufferNumber = 0;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	}

	if (ShaderUsesBuffer(m_vertexName, "Camera"))
	{
		result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto camPtr = (CameraBufferType*)mappedResource.pData;
		camPtr->cameraPosition = params->camera.cameraPosition;
		deviceContext->Unmap(m_cameraBuffer, 0);
		bufferNumber = 2;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
	}

	if (ShaderUsesBuffer(m_vertexName, "Fog"))
	{
		result = deviceContext->Map(m_fogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto fogPtr = (FogBufferType*)mappedResource.pData;
		fogPtr->fogStart = params->fog.fogStart;
		fogPtr->fogEnd = params->fog.fogEnd;
		deviceContext->Unmap(m_fogBuffer, 0);
		bufferNumber = 3;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_fogBuffer);
	}

	if (ShaderUsesBuffer(m_vertexName, "Clip"))
	{
		result = deviceContext->Map(m_clipBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto matrixPtr = (ClipPlaneBufferType*)mappedResource.pData;
		matrixPtr->clipPlane = params->clip.clipPlane;
		deviceContext->Unmap(m_clipBuffer, 0);
		bufferNumber = 4;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_clipBuffer);
	}

	if (ShaderUsesBuffer(m_vertexName, "Reflection") && REFLECTION_ENABLED)
	{
		result = deviceContext->Map(m_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto ptr = (ReflectionBufferType*)mappedResource.pData;
		XMMATRIX matrix = params->reflection.reflectionMatrix;
		ptr->reflectionMatrix = XMMatrixTranspose(matrix);
		deviceContext->Unmap(m_reflectionBuffer, 0);
		bufferNumber = 5;
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_reflectionBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "Util"))
	{
		result = deviceContext->Map(m_utilBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto utilPtr = (UtilBufferType*)mappedResource.pData;
		utilPtr->time = params->utils.time;
		deviceContext->Unmap(m_utilBuffer, 0);
		bufferNumber = 0;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_utilBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "LightPosition"))
	{
		XMFLOAT4* lightPosition = params->lightPos.lightPosition;

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
		XMFLOAT4* diffuseColor = params->lightColor.diffuseColor;

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
		result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto lightPtr = (LightBufferType*)mappedResource.pData;
		lightPtr->ambientColor = params->light.ambientColor;
		lightPtr->diffuseColor = params->light.diffuseColor;
		lightPtr->lightDirection = params->light.lightDirection;
		lightPtr->specularColor = params->light.specularColor;
		lightPtr->specularPower = params->light.specularPower;
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
		pixelPtr->pixelColor = params->pixel.pixelColor;
		deviceContext->Unmap(m_pixelBuffer, 0);
		bufferNumber = 0;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "TexTranslation"))
	{
		result = deviceContext->Map(m_texTransBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto pixelPtr = (TexTranslationBufferType*)mappedResource.pData;
		pixelPtr->translation = params->textureTranslation.translation;
		pixelPtr->timeMultiplier = params->textureTranslation.timeMultiplier;
		deviceContext->Unmap(m_texTransBuffer, 0);
		bufferNumber = 3;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_texTransBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "Alpha"))
	{
		result = deviceContext->Map(m_alphaBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto ptr = (AlphaBufferType*)mappedResource.pData;
		ptr->alphaBlend = params->alpha.alphaBlend;
		deviceContext->Unmap(m_alphaBuffer, 0);
		bufferNumber = 4;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_alphaBuffer);
	}

	if (ShaderUsesBuffer(m_fragName, "Water"))
	{
		result = deviceContext->Map(m_waterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;
		auto ptr = (WaterBufferType*)mappedResource.pData;
		ptr->reflectRefractScale = params->water.reflectRefractScale;
		deviceContext->Unmap(m_waterBuffer, 0);
		bufferNumber = 5;
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_waterBuffer);
	}

	return true;
}

void ShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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

bool ShaderClass::ShaderUsesBuffer(std::string shader, std::string buffer)
{
	if (shader == "Light.vs") {
		return buffer == "Matrix" ||
			buffer == "Camera";
	}

	if (shader == "Fog.vs") {
		return buffer == "Matrix" ||
			buffer == "Camera" ||
			buffer == "Fog" ||
			buffer == "Clip";
	}

	if (shader == "Simple.vs")
	{
		return buffer == "Matrix";
	}

	if (shader == "Reflect.vs" || shader == "Water.vs")
	{
		return buffer == "Matrix" ||
			buffer == "Camera" ||
			buffer == "Fog" ||
			buffer == "Clip" ||
			buffer == "Reflection";
	}

	if (shader == "MultiTex.ps" || shader == "Fog.ps" || shader == "Reflect.ps" || shader == "Fractal.ps") {
		return buffer == "LightColor" ||
			buffer == "Util" ||
			buffer == "Light" ||
			buffer == "LightPosition" ||
			buffer == "TexTranslation" ||
			buffer == "Alpha";
	}

	if (shader == "Water.ps")
	{
		return buffer == "LightColor" ||
			buffer == "Util" ||
			buffer == "Light" ||
			buffer == "LightPosition" ||
			buffer == "TexTranslation" ||
			buffer == "Alpha" ||
			buffer == "Water";
	}

	if (shader == "Font.ps") {
		return buffer == "Pixel";
	}

	if (shader == "2D.ps") {
		return buffer == "Util";
	}

	return false;
}