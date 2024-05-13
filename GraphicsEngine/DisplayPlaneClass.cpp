#include "DisplayPlaneClass.h"

DisplayPlaneClass::DisplayPlaneClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}

DisplayPlaneClass::~DisplayPlaneClass()
{
}

bool DisplayPlaneClass::Initialize(ID3D11Device* device, float width, float height, RenderTextureClass* rendTex, ShaderClass* shader, const char* name, CameraClass* cam)
{
    m_RenderTexture = rendTex;
    m_Shader = shader;
    m_TexSet = new TextureSetClass;
    m_NameIdentifier = name;

    if (cam)
        m_Camera = cam;
    else
    {
        m_Camera = new CameraClass();
        m_ownCamera = true;
    }
        
    SetScale(1, 1, 1);

    return InitializeBuffers(device, width, height);
}

void DisplayPlaneClass::Shutdown()
{
    if (m_TexSet)
    {
        m_TexSet->Shutdown();
        delete m_TexSet;
        m_TexSet = 0;
    }

    if (m_Camera && m_ownCamera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    ShutdownBuffers();
}

bool DisplayPlaneClass::Render(ID3D11DeviceContext* deviceContext, ShaderClass::ShaderParamsGlobalType* params)
{
    bool result;

    m_shaderUniformData.matrix.world = GetWorldMatrix();

    m_TexSet->Add(m_RenderTexture->GetShaderResourceView(), 0);

    RenderBuffers(deviceContext);

    result = m_Shader->Render(deviceContext, GetIndexCount(), m_TexSet, params, &m_shaderUniformData);
    if (!result)
        return false;

    return true;
}

XMMATRIX DisplayPlaneClass::GetWorldMatrix()
{
    XMMATRIX scaleMatrix = XMMatrixScaling(m_ScaleX, m_ScaleY, m_ScaleZ);
    XMMATRIX translateMatrix = XMMatrixTranslation(m_PosX, m_PosY, m_PosZ);
    XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(m_RotX * (float)DEG_TO_RAD, m_RotY * (float)DEG_TO_RAD, m_RotZ * (float)DEG_TO_RAD);

    XMMATRIX srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
    return XMMatrixMultiply(srMatrix, translateMatrix);
}

int DisplayPlaneClass::GetIndexCount()
{
    return m_indexCount;
}

bool DisplayPlaneClass::InitializeBuffers(ID3D11Device* device, float width, float height)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    m_vertexCount = 6;
    m_indexCount = m_vertexCount;

    vertices = new VertexType[m_vertexCount];
    indices = new unsigned long[m_indexCount];

    // First triangle.
    vertices[0].position = XMFLOAT3(-width, height, 0.0f);  // Top left.
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(width, -height, 0.0f);  // Bottom right.
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(-width, -height, 0.0f);  // Bottom left.
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    // Second triangle.
    vertices[3].position = XMFLOAT3(-width, height, 0.0f);  // Top left.
    vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = XMFLOAT3(width, height, 0.0f);  // Top right.
    vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = XMFLOAT3(width, -height, 0.0f);  // Bottom right.
    vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

    // Load the index array with data.
    for (int i = 0; i < m_indexCount; i++)
    {
        indices[i] = i;
    }

    // Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
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

    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}


void DisplayPlaneClass::ShutdownBuffers()
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
}

void DisplayPlaneClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DisplayPlaneClass::SetPosition(float  x, float y, float z)
{
    m_PosX = x;
    m_PosY = y;
    m_PosZ = z;
}

void DisplayPlaneClass::SetRotation(float x, float y, float z)
{
    m_RotX = x;
    m_RotY = y;
    m_RotZ = z;
}

void DisplayPlaneClass::SetScale(float x, float y, float z)
{
    m_ScaleX = x;
    m_ScaleY = y;
    m_ScaleZ = z;
}

void DisplayPlaneClass::SetScale(float x)
{
    m_ScaleX = x;
    m_ScaleY = x;
    m_ScaleZ = x;
}

void DisplayPlaneClass::SetCameraPosAndRot(float px, float py, float pz, float rx, float ry, float rz)
{
    m_Camera->SetPosition(px, py, pz);
    m_Camera->SetRotation(rx, ry, rz);
}

XMVECTOR DisplayPlaneClass::GetForwardVector()
{
    XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_RotX * DEG_TO_RAD, m_RotY * DEG_TO_RAD, m_RotZ * DEG_TO_RAD);
    return rotationMatrix.r[2];
}