#include "ShaderManagerClass.h"

ShaderManagerClass::ShaderManagerClass()
{
    m_TextureShader = 0;
}


ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{
}


ShaderManagerClass::~ShaderManagerClass()
{
}


bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;

    m_TextureShader = new TextureShaderClass;
    result = m_TextureShader->Initialize(device, hwnd);
    if (!result)
        return false;

    return true;
}

void ShaderManagerClass::Shutdown()
{
    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }
}

bool ShaderManagerClass::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture)
{
    return m_TextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}