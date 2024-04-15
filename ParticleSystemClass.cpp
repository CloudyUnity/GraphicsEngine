#include "ParticleSystemClass.h"

ParticleSystemClass::ParticleSystemClass()
{
	m_Shader = 0;
	
	m_PosX = 0;
	m_PosY = 0;
	m_PosZ = 0;
	m_RotX = 0;
	m_RotY = 0;
	m_RotZ = 0;
	m_ScaleX = 1;
	m_ScaleY = 1;
	m_ScaleZ = 1;
	
	m_Textures = 0;

    m_backCullingDisabled = true;
}

ParticleSystemClass::~ParticleSystemClass()
{
}

bool ParticleSystemClass::Initialize(ID3D11Device* device, ParticleSystemData data, ShaderClass* shader, TextureSetClass* texSet, std::string name)
{
	m_Shader = shader;
	m_Textures = texSet;
	m_NameIdentifier = name;
    m_data = data;    

	SetScale(1, 1, 1);

    m_particles = new Particle[m_data.maxParticles];

    for (int i = 0; i < m_data.maxParticles; i++)
    {
        m_particles[i].active = false;
    }

    m_currentParticleCount = 0;
    m_accumulatedTime = 0;

    return InitializeBuffers(device);
}

bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    m_vertexCount = m_data.maxParticles * 6;
    m_indexCount = m_vertexCount;

    m_vertices = new VertexParticleType[m_vertexCount];
    memset(m_vertices, 0, (sizeof(VertexParticleType) * m_vertexCount));

    indices = new unsigned long[m_indexCount];

    // Load the index array with data.
    for (int i = 0; i < m_indexCount; i++)
    {
        indices[i] = i;
    }

    // Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexParticleType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
        return false;

    // Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
        return false;

    delete[] indices;
    indices = 0;

    return true;
}

bool ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext, ShaderClass::ShaderParamsGlobalType* params, ShaderClass* overwriteShader)
{
	XMMATRIX scaleMatrix = XMMatrixScaling(m_ScaleX, m_ScaleY, m_ScaleZ);
	XMMATRIX translateMatrix = XMMatrixTranslation(m_PosX, m_PosY, m_PosZ);
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(m_RotX * (float)DEG_TO_RAD, m_RotY * (float)DEG_TO_RAD, m_RotZ * (float)DEG_TO_RAD);

	XMMATRIX srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
    m_shaderUniformData.matrix.world = XMMatrixMultiply(srMatrix, translateMatrix);

    RenderBuffers(deviceContext);

	if (overwriteShader)
	{
		return overwriteShader->Render(deviceContext, m_indexCount, nullptr, params, &m_shaderUniformData);
	}

	return m_Shader->Render(deviceContext, m_indexCount, m_Textures, params, &m_shaderUniformData);
}

void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexParticleType);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ParticleSystemClass::Shutdown()
{
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    if (m_particles)
    {
        delete[] m_particles;
        m_particles = 0;
    }

    if (m_vertices)
    {
        delete[] m_vertices;
        m_vertices = 0;
    }
}

bool ParticleSystemClass::Frame(ID3D11DeviceContext* deviceContext, float frameTime)
{
    KillParticles();

    m_totalTime += frameTime;
    m_accumulatedTime += frameTime;

    float secondsPerParticle = 1.0f / m_data.particlesPerSecond;    
    while (!m_disabled && m_currentParticleCount < (m_data.maxParticles - 1))
    {
        bool validTime = m_accumulatedTime >= secondsPerParticle;
        if (!validTime)
            break;
        
        m_accumulatedTime -= secondsPerParticle;
        EmitParticles();
    }

    UpdateParticles(frameTime);

    return UpdateBuffers(deviceContext);
}

void ParticleSystemClass::EmitParticles()
{
    m_currentParticleCount++;

    float positionZ = m_data.posZ + GetRandomNumber() * m_data.posDevZ;

    // We will sort using Z depth so we need to find where in the list the particle should be inserted.
    int index = 0;
    while (index < m_data.maxParticles)
    {
        bool inactiveParticle = m_particles[index].active == false;
        bool closerParticle = m_particles[index].positionZ < positionZ;
        if (inactiveParticle || closerParticle)
            break;

        index++;
    }

    // Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
    for (int i = m_currentParticleCount; i > index; i--)
        m_particles[i] = m_particles[i - 1];

    m_particles[index].positionX = m_data.posX + GetRandomNumber() * m_data.posDevX;
    m_particles[index].positionY = m_data.posY + GetRandomNumber() * m_data.posDevY;
    m_particles[index].positionZ = positionZ;

    m_particles[index].red = m_data.colR + (GetRandomNumber() + 0.5f) * m_data.colRDev;
    m_particles[index].green = m_data.colG + (GetRandomNumber() + 0.5f) * m_data.colGDev;
    m_particles[index].blue = m_data.colB + (GetRandomNumber() + 0.5f) * m_data.colBDev;

    m_particles[index].speed = m_data.speed + GetRandomNumber() * m_data.speedDev;
    m_particles[index].size = m_data.size + GetRandomNumber() * m_data.sizeDev;

    m_particles[index].active = true;

    m_particles[index].startTime = m_totalTime;
    m_particles[index].lifetime = m_data.lifetime + GetRandomNumber() * m_data.lifetimeDev;
}

void ParticleSystemClass::UpdateParticles(float frameTime)
{
    for (int i = 0; i < m_currentParticleCount; i++)
    {
        m_particles[i].positionX += m_data.velocityX * (m_particles[i].speed * frameTime);
        m_particles[i].positionY += m_data.velocityY * (m_particles[i].speed * frameTime);
        m_particles[i].positionZ += m_data.velocityZ * (m_particles[i].speed * frameTime);
    }
}

void ParticleSystemClass::KillParticles()
{
    for (int i = 0; i < m_data.maxParticles; i++)
    {
        if (!m_particles[i].active)
            continue;

        if (m_totalTime - m_particles[i].startTime < m_particles[i].lifetime)
            continue;

        m_particles[i].active = false;
        m_currentParticleCount--;

        // Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
        for (int j = i; j < m_data.maxParticles - 1; j++)
            m_particles[j] = m_particles[j + 1];
    }
}

bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
    int index;
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    VertexParticleType* verticesPtr;

    // Initialize vertex array to zeros at first.
    memset(m_vertices, 0, (sizeof(VertexParticleType) * m_vertexCount));

    // Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
    index = 0;

    for (int i = 0; i < m_currentParticleCount; i++)
    {
        float x = m_particles[i].positionX;
        float y = m_particles[i].positionY;
        float z = m_particles[i].positionZ;

        float r = m_particles[i].red;
        float g = m_particles[i].green;
        float b = m_particles[i].blue;

        float t = (m_totalTime - m_particles[i].startTime) / (m_particles[i].lifetime);

        float s = m_particles[i].size * MathUtilsClass::LerpCurveWithIndex(m_data.sizeOverLifetimeCurveIndex, t);

        // Bottom left.
        m_vertices[index].position = XMFLOAT3(x - s, y - s, z);
        m_vertices[index].texture = XMFLOAT2(0.0f, 1.0f);
        m_vertices[index].color = XMFLOAT4(r, g, b, 1.0f);
        index++;

        // Top left.
        m_vertices[index].position = XMFLOAT3(x - s, y + s, z);
        m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
        m_vertices[index].color = XMFLOAT4(r, g, b, 1.0f);
        index++;

        // Bottom right.
        m_vertices[index].position = XMFLOAT3(x + s, y - s, z);
        m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
        m_vertices[index].color = XMFLOAT4(r, g, b, 1.0f);
        index++;

        // Bottom right.
        m_vertices[index].position = XMFLOAT3(x + s, y - s, z);
        m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
        m_vertices[index].color = XMFLOAT4(r, g, b, 1.0f);
        index++;

        // Top left.
        m_vertices[index].position = XMFLOAT3(x - s, y + s, z);
        m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
        m_vertices[index].color = XMFLOAT4(r, g, b, 1.0f);
        index++;

        // Top right.
        m_vertices[index].position = XMFLOAT3(x + s, y + s, z);
        m_vertices[index].texture = XMFLOAT2(1.0f, 0.0f);
        m_vertices[index].color = XMFLOAT4(r, g, b, 1.0f);
        index++;
    }

    result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;

    verticesPtr = (VertexParticleType*)mappedResource.pData;

    memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexParticleType) * m_vertexCount));
    deviceContext->Unmap(m_vertexBuffer, 0);

    return true;
}

void ParticleSystemClass::SetPosition(float x, float y, float z)
{
    m_PosX = x;
    m_PosY = y;
    m_PosZ = z;
}

void ParticleSystemClass::SetRotation(float x, float y, float z)
{
    m_RotX = x;
    m_RotY = y;
    m_RotZ = z;
}

void ParticleSystemClass::SetScale(float x, float y, float z)
{
    m_ScaleX = x;
    m_ScaleY = y;
    m_ScaleZ = z;    
}

void ParticleSystemClass::SetScale(float s)
{
    m_ScaleX = s;
    m_ScaleY = s;
    m_ScaleZ = s;
}

float ParticleSystemClass::GetRandomNumber()
{
    return (((float)rand() - (float)rand()) / RAND_MAX);
}

void ParticleSystemClass::SetBackCulling(bool enabled)
{
    m_backCullingDisabled = !enabled;
}
