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

bool ShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, TextureSetClass* textures, ShaderParameters* parameters)
{
	bool result = SetShaderParameters(deviceContext, textures, parameters);
	if (!result)
		return false;

	RenderShader(deviceContext, indexCount);
	return true;
}

bool ShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, bool clampSamplerMode)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;	
	unsigned int numElements;

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	if (sizeof(ShaderParameters) % 16 != 0)
	{
		MessageBox(hwnd, vsFilename, L"Constant Buffers are wrong size", MB_OK);
		return false;
	}

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

	D3D11_INPUT_ELEMENT_DESC polygonLayout[10];

	if (m_vertexName == "Particle.vs")
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
		TryCreateBuffer(device, bufferDesc, m_matrixBuffer, sizeof(MatrixBufferType), m_vertexName, "Matrix") &&
		TryCreateBuffer(device, bufferDesc, m_fogBuffer, sizeof(FogBufferType), m_vertexName, "Fog") &&
		TryCreateBuffer(device, bufferDesc, m_clipBuffer, sizeof(ClipPlaneBufferType), m_vertexName, "Clip") &&
		TryCreateBuffer(device, bufferDesc, m_cameraBuffer, sizeof(CameraBufferType), m_vertexName, "Camera") &&
		TryCreateBuffer(device, bufferDesc, m_reflectionBuffer, sizeof(ReflectionBufferType), m_vertexName, "Reflection") &&
		TryCreateBuffer(device, bufferDesc, m_utilBuffer, sizeof(UtilBufferType), m_fragName, "Util") &&
		TryCreateBuffer(device, bufferDesc, m_lightColorBuffer, sizeof(LightColorBufferType), m_fragName, "LightColor") &&
		TryCreateBuffer(device, bufferDesc, m_lightBuffer, sizeof(LightBufferType), m_fragName, "Light") &&
		TryCreateBuffer(device, bufferDesc, m_pixelBuffer, sizeof(PixelBufferType), m_fragName, "Pixel") &&
		TryCreateBuffer(device, bufferDesc, m_texTransBuffer, sizeof(TexTranslationBufferType), m_fragName, "TexTranslation") &&
		TryCreateBuffer(device, bufferDesc, m_alphaBuffer, sizeof(AlphaBufferType), m_fragName, "Alpha") &&		
		TryCreateBuffer(device, bufferDesc, m_waterBuffer, sizeof(WaterBufferType), m_fragName, "Water") &&		
		TryCreateBuffer(device, bufferDesc, m_fireBuffer, sizeof(FireBufferType), m_fragName, "Fire") &&		
		TryCreateBuffer(device, bufferDesc, m_shadowBuffer, sizeof(ShadowBufferType), m_fragName, "Shadow") &&		
		TryCreateBuffer(device, bufferDesc, m_blurBuffer, sizeof(BlurBufferType), m_fragName, "Blur") &&		
		TryCreateBuffer(device, bufferDesc, m_filterBuffer, sizeof(FilterBufferType), m_fragName, "Filter") &&		
		TryCreateBuffer(device, bufferDesc, m_lightPositionBuffer, sizeof(LightPositionBufferType), m_fragName, "LightPosition");

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

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
		return false;

	return true;
}

bool ShaderClass::TryCreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer*& ptr, size_t structSize, string shaderName, string bufferName)
{
	if (!ShaderUsesBuffer(shaderName, bufferName))
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

bool ShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, TextureSetClass* textures, ShaderParameters* params)
{
	bool setVS = true;
	bool setPS = false;

	if (textures)
	{
		int textureCount = textures->GetCount();

		for (int i = 0; i < textureCount; i++)
		{
			ID3D11ShaderResourceView* tex = textures->GetTexture(i);
			deviceContext->PSSetShaderResources(i, 1, &tex);
		}
	}
	
	if (ShaderUsesBuffer(m_vertexName, "Matrix"))
	{		
		MatrixBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_matrixBuffer, &ptr))
			return false;

		ptr->world = XMMatrixTranspose(params->matrix.world);
		ptr->view = XMMatrixTranspose(params->matrix.view);
		ptr->projection = XMMatrixTranspose(params->matrix.projection);

		UnmapBuffer(deviceContext, 0, &m_matrixBuffer, setVS);		
	}

	if (ShaderUsesBuffer(m_vertexName, "Camera"))
	{
		CameraBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_cameraBuffer, &ptr))
			return false;

		ptr->cameraPosition = params->camera.cameraPosition;

		UnmapBuffer(deviceContext, 2, &m_cameraBuffer, setVS);
	}

	if (ShaderUsesBuffer(m_vertexName, "Fog"))
	{
		FogBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_fogBuffer, &ptr))
			return false;

		ptr->fogStart = params->fog.fogStart;
		ptr->fogEnd = params->fog.fogEnd;

		UnmapBuffer(deviceContext, 3, &m_fogBuffer, setVS);
	}

	if (ShaderUsesBuffer(m_vertexName, "Clip"))
	{
		ClipPlaneBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_clipBuffer, &ptr))
			return false;

		ptr->clipPlane = params->clip.clipPlane;

		UnmapBuffer(deviceContext, 4, &m_clipBuffer, setVS);
	}

	if (ShaderUsesBuffer(m_vertexName, "Reflection") && params->reflectionEnabled)
	{
		ReflectionBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_reflectionBuffer, &ptr))
			return false;

		XMMATRIX matrix = params->reflection.reflectionMatrix;
		ptr->reflectionMatrix = XMMatrixTranspose(matrix);

		UnmapBuffer(deviceContext, 5, &m_reflectionBuffer, setVS);
	}

	if (ShaderUsesBuffer(m_fragName, "Util"))
	{
		UtilBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_utilBuffer, &ptr))
			return false;

		ptr->time = params->utils.time;
		ptr->texelSizeX = params->utils.texelSizeX;
		ptr->texelSizeY = params->utils.texelSizeY;

		UnmapBuffer(deviceContext, 0, &m_utilBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_vertexName, "LightPosition"))
	{		
		LightPositionBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_lightPositionBuffer, &ptr))
			return false;

		XMFLOAT4* lightPosition = params->lightPos.lightPosition;

		for (int i = 0; i < NUM_POINT_LIGHTS; i++)
			ptr->lightPosition[i] = lightPosition[i];

		UnmapBuffer(deviceContext, 1, &m_lightPositionBuffer, setVS);
	}

	if (ShaderUsesBuffer(m_fragName, "LightColor"))
	{		
		LightColorBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_lightColorBuffer, &ptr))
			return false;

		XMFLOAT4* diffuseColor = params->lightColor.diffuseColor;

		for (int i = 0; i < NUM_POINT_LIGHTS; i++)
			ptr->diffuseColor[i] = diffuseColor[i];

		UnmapBuffer(deviceContext, 1, &m_lightColorBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "Light"))
	{
		LightBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_lightBuffer, &ptr))
			return false;

		ptr->ambientColor = params->light.ambientColor;
		ptr->diffuseColor = params->light.diffuseColor;
		ptr->lightDirection = params->light.lightDirection;
		ptr->specularColor = params->light.specularColor;
		ptr->specularPower = params->light.specularPower;

		UnmapBuffer(deviceContext, 2, &m_lightBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "Pixel"))
	{
		PixelBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_pixelBuffer, &ptr))
			return false;

		ptr->pixelColor = params->pixel.pixelColor;

		UnmapBuffer(deviceContext, 0, &m_pixelBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "TexTranslation"))
	{
		TexTranslationBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_texTransBuffer, &ptr))
			return false;

		ptr->translation = params->textureTranslation.translation;
		ptr->timeMultiplier = params->textureTranslation.timeMultiplier;

		UnmapBuffer(deviceContext, 3, &m_texTransBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "Alpha"))
	{
		AlphaBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_alphaBuffer, &ptr))
			return false;

		ptr->alphaBlend = params->alpha.alphaBlend;

		UnmapBuffer(deviceContext, 4, &m_alphaBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "Water"))
	{
		WaterBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_waterBuffer, &ptr))
			return false;

		ptr->reflectRefractScale = params->water.reflectRefractScale;

		UnmapBuffer(deviceContext, 5, &m_waterBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "Fire"))
	{
		FireBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_fireBuffer, &ptr))
			return false;

		ptr->distortion1 = params->fire.distortion1;
		ptr->distortion2 = params->fire.distortion2;
		ptr->distortion3 = params->fire.distortion3;
		ptr->distortionScale = params->fire.distortionScale;
		ptr->distortionBias = params->fire.distortionBias;

		UnmapBuffer(deviceContext, 1, &m_fireBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "Shadow"))
	{
		ShadowBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_shadowBuffer, &ptr))
			return false;

		ptr->shadowView = XMMatrixTranspose(params->shadow.shadowView);
		ptr->shadowProj = XMMatrixTranspose(params->shadow.shadowProj);

		for (int i = 0; i < NUM_POISSON_SAMPLES; i++)
			ptr->poissonDisk[i] = params->shadow.poissonDisk[i];

		ptr->usingShadows = params->shadow.usingShadows;
		ptr->poissonSpread = params->shadow.poissonSpread;
		ptr->shadowBias = params->shadow.shadowBias;
		ptr->shadowCutOff = params->shadow.shadowCutOff;

		UnmapBuffer(deviceContext, 5, &m_shadowBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "Blur"))
	{
		BlurBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_blurBuffer, &ptr))
			return false;

		ptr->blurMode = params->blur.blurMode;

		for (int i = 0; i < BLUR_SAMPLE_SPREAD; i++)
			ptr->weights[i] = params->blur.weights[i];

		UnmapBuffer(deviceContext, 1, &m_blurBuffer, setPS);
	}

	if (ShaderUsesBuffer(m_fragName, "Filter"))
	{
		FilterBufferType* ptr;
		if (!TryMapBuffer(deviceContext, &m_filterBuffer, &ptr))
			return false;

		ptr->grainEnabled = params->filter.grainEnabled;
		ptr->monochromeEnabled = params->filter.monochromeEnabled;
		ptr->sharpnessEnabled = params->filter.sharpnessEnabled;
		ptr->chromaticEnabled = params->filter.chromaticEnabled;

		ptr->sharpnessKernalN = params->filter.sharpnessKernalN;
		ptr->sharpnessKernalP = params->filter.sharpnessKernalP;
		ptr->sharpnessStrength = params->filter.sharpnessStrength;

		ptr->vignetteEnabled = params->filter.vignetteEnabled;
		ptr->vignetteSmoothness = params->filter.vignetteSmoothness;
		ptr->vignetteStrength = params->filter.vignetteStrength;
		ptr->grainIntensity = params->filter.grainIntensity;

		UnmapBuffer(deviceContext, 1, &m_filterBuffer, setPS);
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
	if (shader == "Light.vs" || shader == "Particle.vs") {
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

	if (shader == "MultiTex.ps" || shader == "Reflect.ps" || shader == "Fractal.ps") {
		return buffer == "LightColor" ||
			buffer == "Util" ||
			buffer == "Light" ||
			buffer == "LightPosition" ||
			buffer == "TexTranslation" ||
			buffer == "Alpha";
	}

	if (shader == "Fog.ps")
	{
		return buffer == "LightColor" ||
			buffer == "Util" ||
			buffer == "Light" ||
			buffer == "LightPosition" ||
			buffer == "TexTranslation" ||
			buffer == "Shadow" ||
			buffer == "Alpha";
	}

	if (shader == "Depth.vs")
	{
		return buffer == "Matrix";
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

	if (shader == "Filter.ps")
	{
		return buffer == "Util" ||
			buffer == "Filter";
	}

	if (shader == "Blur.ps")
	{
		return buffer == "Util" ||
			buffer == "Blur";
	}

	if (shader == "Fire.ps")
	{
		return buffer == "Util" ||
			buffer == "Fire";
	}

	if (shader == "Skybox.vs")
	{
		return buffer == "Matrix";
	}

	return false;
}

void ShaderClass::UnmapBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer, bool setToVertexShader)
{
	deviceContext->Unmap(*buffer, 0);
	if (setToVertexShader)
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, buffer);
	else
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, buffer);
}
