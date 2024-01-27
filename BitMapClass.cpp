#include "BitMapClass.h"

BitmapClass::BitmapClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_Textures = 0;
    m_renderX = 0;
    m_renderY = 0;
}


BitmapClass::BitmapClass(const BitmapClass& other)
{
}


BitmapClass::~BitmapClass()
{
}


bool BitmapClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, char* textureFilename)
{    
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    m_frameTime = 0;

    bool result;

    result = InitializeBuffers(device);
    if (!result)
        return false;

    result = LoadTextures(device, deviceContext, textureFilename);
    if (!result)
        return false;

    SetTextureFrame(0);
    return true;
}

void BitmapClass::Shutdown()
{
    ReleaseTextures();
    ShutdownBuffers();    
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext)
{
    bool result = UpdateBuffers(deviceContext);
    if (!result)
        return false;

    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(deviceContext);

    return true;
}

void BitmapClass::Update(float frameTime)
{
    m_frameTime += frameTime;

    if (m_frameTime >= m_cycleTime)
    {
        m_frameTime -= m_cycleTime;

        m_currentTexture++;
        if (m_currentTexture == m_textureCount)
            m_currentTexture = 0;
        SetTextureFrame(m_currentTexture);
    }
}

int BitmapClass::GetIndexCount()
{
    return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
    return m_AllTextureFrames[m_currentTexture].GetSRV();
}

void BitmapClass::SetTextureFrame(int frame) 
{
    m_Textures = &m_AllTextureFrames[frame];
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType2D* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    
    // Initialize the previous rendering position to negative one.
    m_prevPosX = -1;
    m_prevPosY = -1;

    m_vertexCount = 6;
    m_indexCount = m_vertexCount;
    vertices = new VertexType2D[m_vertexCount];
    indices = new unsigned long[m_indexCount];

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, (sizeof(VertexType2D) * m_vertexCount));

    // Load the index array with data.
    for (int i = 0; i < m_indexCount; i++)
    {
        indices[i] = i;
    }

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType2D) * m_vertexCount;
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

void BitmapClass::ShutdownBuffers()
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

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContent)
{
    if (m_vertexCount < 6)
        return false;

    float left, right, top, bottom;
    VertexType2D* vertices;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    VertexType2D* dataPtr;
    HRESULT result;

    if ((m_prevPosX == m_renderX) && (m_prevPosY == m_renderY))
        return true;

    m_prevPosX = m_renderX;
    m_prevPosY = m_renderY;

    vertices = new VertexType2D[m_vertexCount];

    left = (float)((m_screenWidth / 2) * -1) + (float)m_renderX;
    right = left + (float)m_bitmapWidth;
    top = (float)(m_screenHeight / 2) - (float)m_renderY;
    bottom = top - (float)m_bitmapHeight;

    // First triangle.
    vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    // Second triangle.
    vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
    vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
    vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
    vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

    result = deviceContent->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;
    dataPtr = (VertexType2D*)mappedResource.pData;
    memcpy(dataPtr, (void*)vertices, (sizeof(VertexType2D) * m_vertexCount));
    deviceContent->Unmap(m_vertexBuffer, 0);

    dataPtr = 0;
    delete[] vertices;
    vertices = 0;

    return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexType2D);
    unsigned int offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool BitmapClass::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
    char textureFilename[128];
    ifstream fin;
    char input;
    bool result;

    fin.open(filename);
    if (fin.fail())
        return false;

    fin >> m_textureCount;
    fin.get(input);

    m_AllTextureFrames = new TextureClass[m_textureCount];    

    for (int i = 0; i < m_textureCount; i++)
    {
        int j = 0;
        fin.get(input);
        while (input != '\n')
        {
            textureFilename[j] = input;
            j++;
            fin.get(input);
        }
        textureFilename[j] = '\0'; // Null terminated array

        result = m_AllTextureFrames[i].Initialize(device, deviceContext, textureFilename);
        if (!result)
            return false;
    }
    
    fin >> m_cycleTime;
    m_cycleTime = m_cycleTime * 0.001f;

    fin.close();

    // Get the dimensions of the first texture and use that as the dimensions of the 2D sprite images.
    m_bitmapWidth = m_AllTextureFrames[0].GetWidth();
    m_bitmapHeight = m_AllTextureFrames[0].GetHeight();

    m_currentTexture = 0;

    return true;
}

void BitmapClass::ReleaseTextures()
{
    if (m_AllTextureFrames)
    {
        for (int i = 0; i < m_textureCount; i++)
        {
            m_AllTextureFrames[i].Shutdown();
        }

        delete[] m_AllTextureFrames;
        m_AllTextureFrames = 0;
    }
}


void BitmapClass::SetRenderLocation(int x, int y)
{
    m_renderX = x;
    m_renderY = y;
}
