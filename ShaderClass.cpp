#include "shaderclass.h"

ShaderClass::ShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
}

ShaderClass::~ShaderClass()
{
}

bool ShaderClass::Initialize(ID3D11Device* device, HWND hwnd, char* vertexName, char* fragName, bool clampSamplerMode)
{
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

	return InitializeShader(device, hwnd, vsFilename, psFilename, clampSamplerMode);
}

void ShaderClass::Shutdown()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	for (auto b : m_bufferPtrListVertex)
	{
		if (b)
		{
			b->Release();
		}
	}

	for (auto b : m_bufferPtrListFragment)
	{
		if (b)
		{
			b->Release();
		}
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}
}

bool ShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, TextureSetClass* textures, ShaderParamsGlobalType* globalParams, ShaderParamsObjectType* objectParams)
{
	bool result = SetShaderParameters(deviceContext, textures, globalParams, objectParams);
	if (!result)
		return false;

	RenderShader(deviceContext, indexCount);
	return true;
}

bool ShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, bool clampSamplerMode)
{
	HRESULT hresult;
	bool result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	unsigned int numElements;

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	if (sizeof(ShaderParamsGlobalType) % 16 != 0)
	{
		MessageBox(hwnd, vsFilename, L"Constant Buffers are wrong size", MB_OK);
		return false;
	}

	hresult = D3DCompileFromFile(vsFilename, NULL, NULL, "VS_MAIN", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(hresult))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
			return false;
		}

		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	hresult = D3DCompileFromFile(psFilename, NULL, NULL, "PS_MAIN", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(hresult))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
			return false;
		}

		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	hresult = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(hresult))
		return false;

	hresult = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(hresult))
		return false;

	result = SaveCBufferInfo(vertexShaderBuffer, pixelShaderBuffer);
	if (!result)
		return false;

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.

	D3D11_INPUT_ELEMENT_DESC polygonLayout[10];

	if (m_vertexName == "Particle.vs") // Is this neccessary?
	{
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

		polygonLayout[2].SemanticName = "COLOR";
		polygonLayout[2].SemanticIndex = 0;
		polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[2].InputSlot = 0;
		polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[2].InstanceDataStepRate = 0;

		numElements = 3;
	}
	else
	{
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

		numElements = 5;
	}

	hresult = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(hresult))
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

	for (auto vBuffer : m_cbufferListVertex)
	{
		result = TryCreateBufferVertex(device, bufferDesc, vBuffer);
		if (!result)
			return false;
	}

	for (auto pBuffer : m_cbufferListFragment)
	{
		result = TryCreateBufferFragment(device, bufferDesc, pBuffer);
		if (!result)
			return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	if (clampSamplerMode)
	{
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	}
	else
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

	hresult = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(hresult))
		return false;

	return true;
}

bool ShaderClass::TryCreateBufferVertex(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, string bufferName)
{
	int bufferIndex = UsesCBufferVertex(bufferName);
	if (bufferIndex == -1)
		return false;

	ID3D11Buffer* ptr;
	bufferDesc.ByteWidth = (UINT)m_cbufferSizeListVertex.at(bufferIndex);
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &ptr);

	m_bufferPtrListVertex.push_back(ptr);

	return !FAILED(result);
}

bool ShaderClass::TryCreateBufferFragment(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, string bufferName)
{
	int bufferIndex = UsesCBufferFragment(bufferName);
	if (bufferIndex == -1)
		return false;

	ID3D11Buffer* ptr;
	bufferDesc.ByteWidth = (UINT)m_cbufferSizeListFragment.at(bufferIndex);
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &ptr);

	m_bufferPtrListFragment.push_back(ptr);

	return !FAILED(result);
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

bool ShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures, ShaderParamsGlobalType* globalParams, ShaderParamsObjectType* objectParams)
{
	bool result;

	if (textures)
	{
		int textureCount = textures->GetCount();

		for (int i = 0; i < textureCount; i++)
		{
			ID3D11ShaderResourceView* tex = textures->GetTexture(i);
			deviceContext->PSSetShaderResources(i, 1, &tex);
		}
	}

	objectParams->matrix.world = XMMatrixTranspose(objectParams->matrix.world);
	objectParams->matrix.view = XMMatrixTranspose(objectParams->matrix.view);
	objectParams->matrix.projection = XMMatrixTranspose(objectParams->matrix.projection);

	globalParams->reflection.reflectionMatrix = XMMatrixTranspose(globalParams->reflection.reflectionMatrix);
	globalParams->shadow.shadowView = XMMatrixTranspose(globalParams->shadow.shadowView);
	globalParams->shadow.shadowProj = XMMatrixTranspose(globalParams->shadow.shadowProj);
	
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
		SetShaderCBuffer(deviceContext, objectParams->filter, "FilterBuffer");
	if (!result)
	{
		LogClass::Log("[!!!] Failed to set shader parameters");
		return false;
	}

	if (globalParams->reflectionEnabled)
	{
		result = SetShaderCBuffer(deviceContext, globalParams->reflection, "ReflectionBuffer");
		if (!result)
		{
			LogClass::Log("[!!!] Failed to set shader parameters (Reflection Buffer)");
			return false;
		}
	}

	globalParams->reflection.reflectionMatrix = XMMatrixTranspose(globalParams->reflection.reflectionMatrix);
	globalParams->shadow.shadowView = XMMatrixTranspose(globalParams->shadow.shadowView);
	globalParams->shadow.shadowProj = XMMatrixTranspose(globalParams->shadow.shadowProj);

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

bool ShaderClass::SaveCBufferInfo(ID3D10Blob* vertexBlob, ID3D10Blob* pixelBlob)
{
	HRESULT result;

	ID3D11ShaderReflection* vReflection = nullptr, * pReflection = nullptr;

	result = D3DReflect(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vReflection);
	if (FAILED(result))
		return false;

	result = D3DReflect(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection);
	if (FAILED(result))
		return false;

	D3D11_SHADER_DESC vDesc, pDesc;
	vReflection->GetDesc(&vDesc);
	pReflection->GetDesc(&pDesc);

	for (UINT i = 0; i < vDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* vBuffer = vReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		vBuffer->GetDesc(&bufferDesc);
		m_cbufferListVertex.push_back(bufferDesc.Name);
		m_cbufferSizeListVertex.push_back(bufferDesc.Size);
	}

	for (UINT i = 0; i < pDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pBuffer = pReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		pBuffer->GetDesc(&bufferDesc);
		m_cbufferListFragment.push_back(bufferDesc.Name);
		m_cbufferSizeListFragment.push_back(bufferDesc.Size);
	}

	if (pReflection)
		pReflection->Release();

	if (vReflection)
		vReflection->Release();

	return true;
}

int ShaderClass::UsesCBufferVertex(string cbufferName)
{
	auto it = std::find(m_cbufferListVertex.begin(), m_cbufferListVertex.end(), cbufferName);
	if (it == m_cbufferListVertex.end())
		return -1;

	return it - m_cbufferListVertex.begin();
}

int ShaderClass::UsesCBufferFragment(string cbufferName)
{
	auto it = std::find(m_cbufferListFragment.begin(), m_cbufferListFragment.end(), cbufferName);
	if (it == m_cbufferListFragment.end())
		return -1;

	return it - m_cbufferListFragment.begin();
}

void ShaderClass::UnmapVertexBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer)
{
	deviceContext->Unmap(*buffer, 0);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, buffer);
}

void ShaderClass::UnmapFragmentBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer)
{
	deviceContext->Unmap(*buffer, 0);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, buffer);
}
