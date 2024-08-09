#include "TextClass.h"

TextClass::TextClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;    

    m_PosX = 0;
    m_PosY = 0;
    m_RotZ = 0;
    m_ScaleX = 1;
    m_ScaleY = 1;

    m_Shader = 0;
    m_TexSet = 0;
    m_deviceContext = 0;
    m_font = 0;
    m_indexCount = 0;
    m_maxLength = 0;
    m_screenHeight = 0;
    m_screenWidth = 0;
    m_vertexCount = 0;
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, int maxLength, ShaderClass* shader)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    m_maxLength = maxLength;
    m_deviceContext = deviceContext;
    m_ScaleX = 1;
    m_ScaleY = 1;
    m_Shader = shader;

    m_TexSet = new TextureSetClass;

    return InitializeBuffers(device, deviceContext);
}

void TextClass::Shutdown()
{
    ShutdownBuffers();

    if (m_TexSet)
    {
        m_TexSet->Shutdown();
        delete m_TexSet;
        m_TexSet = 0;
    }
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, ShaderClass::ShaderParamsGlobalType* params)
{    
    XMMATRIX scaleMatrix = XMMatrixScaling(m_ScaleX, m_ScaleY, 1.0f);
    XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(0, 0, m_RotZ * 0.0174532925f);
    m_shaderUniformData.matrix.world = XMMatrixMultiply(rotateMatrix, scaleMatrix);

    RenderBuffers(deviceContext);
    m_TexSet->Add(m_font->GetTexture()->GetSRV(), 0);

    return m_Shader->Render(deviceContext, GetIndexCount(), m_TexSet, params, &m_shaderUniformData);
}

int TextClass::GetIndexCount()
{
    return m_indexCount;
}

bool TextClass::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    m_vertexCount = 6 * m_maxLength;
    m_indexCount = m_vertexCount;

    vertices = new VertexType[m_vertexCount];
    indices = new unsigned long[m_indexCount];

    memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

    for (int i = 0; i < m_indexCount; i++)
    {
        indices[i] = i;
    }
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
        return false;

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
        return false;

    delete[] vertices;
    vertices = 0;
    delete[] indices;
    indices = 0;

    return true;
}

void TextClass::ShutdownBuffers()
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

bool TextClass::UpdateText()
{
    int numLetters;
    VertexType* vertices;
    float drawX, drawY;
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    VertexType* verticesPtr;

    numLetters = (int)m_text.size();
    if (numLetters > m_maxLength)
        return false;

    vertices = new VertexType[m_vertexCount];
    memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

    drawX = (float)(((m_screenWidth / 2) * -1) + m_PosX);
    drawY = (float)((m_screenHeight / 2) - m_PosY);

    m_font->BuildVertexArray((void*)vertices, &m_text.front(), drawX, drawY);

    result = m_deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;
    verticesPtr = (VertexType*)mappedResource.pData;
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));
    m_deviceContext->Unmap(m_vertexBuffer, 0);
    
    delete[] vertices;
    vertices = 0;

    return true;
}

void TextClass::SetPosition(int x, int y)
{
    m_PosX = x;
    m_PosY = y;
}

void TextClass::SetFont(FontClass* font)
{
    m_font = font;
}

void TextClass::SetText(char* text)
{
    m_text = text;
}

void TextClass::SetText(const char* text)
{
    m_text = text;
}

void TextClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride, offset;

    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}