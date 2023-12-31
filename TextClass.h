#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

#include "fontclass.h"
#include "TextureShaderClass.h"

class TextClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    TextClass();
    TextClass(const TextClass&);
    ~TextClass();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, int, ShaderClass* shader);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, unordered_map<string, any>);

    int GetIndexCount();

    bool UpdateText();
    void SetPosition(int, int);
    void SetColor(float, float, float);
    void SetFont(FontClass* font);
    void SetText(char* text);
    void SetShader(ShaderClass*);
    void SetScale(float, float);
    void SetRotation(float);
    XMFLOAT4 GetPixelColor();    

private:
    bool InitializeBuffers(ID3D11Device*, ID3D11DeviceContext*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
    int m_screenWidth, m_screenHeight, m_maxLength, m_vertexCount, m_indexCount, m_PosX, m_PosY;
    char* m_text;
    XMFLOAT4 m_pixelColor;
    FontClass* m_font;
    ID3D11DeviceContext* m_deviceContext;
    float m_ScaleX, m_ScaleY;
    float m_RotZ;
    ShaderClass* m_Shader;
    TextureSetClass* m_TexSet;
};

#endif