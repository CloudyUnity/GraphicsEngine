#include "shaderclass.h"

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
	m_alphaBuffer = 0;
	m_fireBuffer = 0;
	m_shadowBuffer = 0;
	m_blurBuffer = 0;
	m_filterBuffer = 0;
	m_tesselationBuffer = 0;
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

	for (auto b : m_bufferList)
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

	bool bufferCreationResult =
		TryCreateBuffer(device, bufferDesc, m_matrixBuffer, sizeof(MatrixBufferType), "MatrixBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_fogBuffer, sizeof(FogBufferType), "FogBuffer") &&
		TryCreateBuffer(device, bufferDesc, m_clipBuffer, sizeof(ClipPlaneBufferType), "ClipPlaneBuffer") &&
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
		TryCreateBuffer(device, bufferDesc, m_lightPositionBuffer, sizeof(LightPositionBufferType), "LightPositionBuffer");

	if (!bufferCreationResult)
		return false;

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

bool ShaderClass::TryCreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer*& ptr, size_t structSize, string bufferName)
{
	if (!UsesCBuffer(bufferName) || ptr != nullptr)
		return true;

	bufferDesc.ByteWidth = (UINT)structSize;
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &ptr);

	m_bufferList.push_back(ptr);

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

	// SetShaderCBuffer(deviceContext, m_matrixBuffer, objectParams->matrix, "MatrixBuffer");

	m_vertexName;
	m_fragName;

	int bufferIndex;

	bufferIndex = UsesCBufferVertex("MatrixBuffer");
	if (bufferIndex != -1)
	{
		MatrixBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_matrixBuffer, &ptr))
			return false;
		*ptr = objectParams->matrix;
		UnmapVertexBuffer(deviceContext, bufferIndex, &m_matrixBuffer);
	}

	bufferIndex = UsesCBufferVertex("CameraBuffer");
	if (bufferIndex != -1)
	{
		CameraBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_cameraBuffer, &ptr))
			return false;
		*ptr = globalParams->camera;
		UnmapVertexBuffer(deviceContext, bufferIndex, &m_cameraBuffer);
	}

	bufferIndex = UsesCBufferVertex("FogBuffer");
	if (bufferIndex != -1)
	{
		FogBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_fogBuffer, &ptr))
			return false;
		*ptr = globalParams->fog;
		UnmapVertexBuffer(deviceContext, bufferIndex, &m_fogBuffer);
	}

	bufferIndex = UsesCBufferVertex("ClipPlaneBuffer");
	if (bufferIndex != -1)
	{
		ClipPlaneBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_clipBuffer, &ptr))
			return false;
		*ptr = objectParams->clip;
		UnmapVertexBuffer(deviceContext, bufferIndex, &m_clipBuffer);
	}

	bufferIndex = UsesCBufferVertex("ReflectionBuffer");
	if (bufferIndex != -1 && globalParams->reflectionEnabled)
	{
		ReflectionBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_reflectionBuffer, &ptr))
			return false;
		*ptr = globalParams->reflection;
		UnmapVertexBuffer(deviceContext, bufferIndex, &m_reflectionBuffer);
	}

	bufferIndex = UsesCBufferFragment("UtilBuffer");
	if (bufferIndex != -1)
	{
		UtilBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_utilBuffer, &ptr))
			return false;
		*ptr = globalParams->utils;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_utilBuffer);
	}

	bufferIndex = UsesCBufferVertex("LightPositionBuffer");
	if (bufferIndex != -1)
	{
		LightPositionBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_lightPositionBuffer, &ptr))
			return false;
		*ptr = globalParams->lightPos;
		UnmapVertexBuffer(deviceContext, bufferIndex, &m_lightPositionBuffer);
	}

	bufferIndex = UsesCBufferFragment("LightColorBuffer");
	if (bufferIndex != -1)
	{
		LightColorBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_lightColorBuffer, &ptr))
			return false;
		*ptr = globalParams->lightColor;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_lightColorBuffer);
	}

	bufferIndex = UsesCBufferFragment("LightBuffer");
	if (bufferIndex != -1)
	{
		LightBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_lightBuffer, &ptr))
			return false;
		*ptr = globalParams->light;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_lightBuffer);
	}

	bufferIndex = UsesCBufferFragment("PixelBuffer");
	if (bufferIndex != -1)
	{
		PixelBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_pixelBuffer, &ptr))
			return false;
		*ptr = objectParams->pixel;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_pixelBuffer);
	}

	bufferIndex = UsesCBufferFragment("TextureTranslationBuffer");
	if (bufferIndex != -1)
	{
		TexTranslationBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_texTransBuffer, &ptr))
			return false;
		*ptr = objectParams->textureTranslation;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_texTransBuffer);
	}

	bufferIndex = UsesCBufferFragment("AlphaBuffer");
	if (bufferIndex != -1)
	{
		AlphaBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_alphaBuffer, &ptr))
			return false;
		*ptr = objectParams->alpha;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_alphaBuffer);
	}

	bufferIndex = UsesCBufferFragment("WaterBuffer");
	if (bufferIndex != -1)
	{
		WaterBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_waterBuffer, &ptr))
			return false;
		*ptr = objectParams->water;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_waterBuffer);
	}

	bufferIndex = UsesCBufferFragment("FireBuffer");
	if (bufferIndex != -1)
	{
		FireBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_fireBuffer, &ptr))
			return false;
		*ptr = objectParams->fire;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_fireBuffer);
	}

	bufferIndex = UsesCBufferFragment("ShadowBuffer");
	if (bufferIndex != -1)
	{
		ShadowBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_shadowBuffer, &ptr))
			return false;
		ptr->shadowView = XMMatrixTranspose(globalParams->shadow.shadowView);
		ptr->shadowProj = XMMatrixTranspose(globalParams->shadow.shadowProj);

		for (int i = 0; i < NUM_POISSON_SAMPLES; i++)
			ptr->poissonDisk[i] = globalParams->shadow.poissonDisk[i];

		ptr->usingShadows = globalParams->shadow.usingShadows;
		ptr->poissonSpread = globalParams->shadow.poissonSpread;
		ptr->shadowBias = globalParams->shadow.shadowBias;
		ptr->shadowCutOff = globalParams->shadow.shadowCutOff;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_shadowBuffer);
	}

	bufferIndex = UsesCBufferFragment("BlurBuffer");
	if (bufferIndex != -1)
	{
		BlurBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_blurBuffer, &ptr))
			return false;
		*ptr = objectParams->blur;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_blurBuffer);
	}

	bufferIndex = UsesCBufferFragment("FilterBuffer");
	if (bufferIndex != -1)
	{
		FilterBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_filterBuffer, &ptr))
			return false;
		*ptr = objectParams->filter;
		UnmapFragmentBuffer(deviceContext, bufferIndex, &m_filterBuffer);
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
	}

	for (UINT i = 0; i < pDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pBuffer = pReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		pBuffer->GetDesc(&bufferDesc);
		m_cbufferListFragment.push_back(bufferDesc.Name);
	}

	if (pReflection)
		pReflection->Release();

	if (vReflection)
		vReflection->Release();

	return true;
}

bool ShaderClass::UsesCBuffer(string cbufferName)
{
	return UsesCBufferVertex(cbufferName) != -1 || UsesCBufferFragment(cbufferName) != -1;
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
