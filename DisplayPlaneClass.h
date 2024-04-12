#ifndef _DISPLAYPLANECLASS_H_
#define _DISPLAYPLANECLASS_H_

#include "d3dclass.h"
#include "RenderTextureClass.h"
#include "shaderclass.h"
#include "CameraClass.h"
#include "IShutdown.h"

class DisplayPlaneClass : public IShutdown
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    DisplayPlaneClass();
    ~DisplayPlaneClass();

    bool Initialize(ID3D11Device*, float, float, RenderTextureClass*, ShaderClass*, const char*, CameraClass* cam = nullptr);
    void Shutdown() override;
    bool Render(ID3D11DeviceContext*, ShaderClass::ShaderParameters*);

    XMMATRIX GetWorldMatrix();

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);
    void SetScale(float, float, float);

    void SetScale(float x);

    void SetCameraPosAndRot(float px, float py, float pz, float rx, float ry, float rz);

    XMVECTOR GetForwardVector();

    int GetIndexCount();

    RenderTextureClass* m_RenderTexture;
    string m_NameIdentifier;
    CameraClass* m_Camera;

    float m_PosX, m_PosY, m_PosZ;
    float m_ScaleX, m_ScaleY, m_ScaleZ;
    float m_RotX, m_RotY, m_RotZ;

private:
    bool InitializeBuffers(ID3D11Device*, float, float);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
    int m_vertexCount, m_indexCount;

    ShaderClass* m_Shader;
    TextureSetClass* m_TexSet;    
    
    bool m_ownCamera;
};

#endif