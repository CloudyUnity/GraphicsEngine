#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_

#include <directxmath.h>
using namespace DirectX;
#include "textureclass.h"
#include "IShutdown.h"
#include <fstream>
using std::ifstream;

class BitmapClass : public IShutdown
{
private:
    struct VertexType2D
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    BitmapClass();
    BitmapClass(const BitmapClass&);
    ~BitmapClass();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*);
    void Shutdown() override;
    bool Render(ID3D11DeviceContext*);

    void Update(float);

    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();

    void SetRenderLocation(int, int);

    TextureClass* m_Textures;

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    bool UpdateBuffers(ID3D11DeviceContext*);
    void RenderBuffers(ID3D11DeviceContext*);
    void SetTextureFrame(int);

    bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*);
    void ReleaseTextures();    

private:
    TextureClass* m_AllTextureFrames;
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
    int m_vertexCount, m_indexCount, m_screenWidth, m_screenHeight, m_bitmapWidth, m_bitmapHeight, m_renderX, m_renderY, m_prevPosX, m_prevPosY;    
    float m_frameTime, m_cycleTime;
    int m_currentTexture, m_textureCount;    
};

#endif