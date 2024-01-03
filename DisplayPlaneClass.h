#ifndef _DISPLAYPLANECLASS_H_
#define _DISPLAYPLANECLASS_H_

#include "d3dclass.h"
#include "RenderTextureClass.h"
#include "TextureShaderClass.h"

class DisplayPlaneClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    DisplayPlaneClass();
    DisplayPlaneClass(const DisplayPlaneClass&);
    ~DisplayPlaneClass();

    bool Initialize(ID3D11Device*, float, float, RenderTextureClass*, ShaderClass*);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, unordered_map<string, any>);

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);
    void SetScale(float, float, float);

    int GetIndexCount();

    RenderTextureClass* m_RenderTexture;

private:
    bool InitializeBuffers(ID3D11Device*, float, float);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
    int m_vertexCount, m_indexCount;
    ShaderClass* m_Shader;
    TextureSetClass* m_TexSet;

    float m_PosX, m_PosY, m_PosZ;
    float m_ScaleX, m_ScaleY, m_ScaleZ;
    float m_RotX, m_RotY, m_RotZ;
};

#endif