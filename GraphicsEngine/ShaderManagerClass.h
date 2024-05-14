#ifndef _SHADERMANAGERCLASS_H_
#define _SHADERMANAGERCLASS_H_

#include "textureshaderclass.h"

class ShaderManagerClass
{
public:
    ShaderManagerClass();
    ShaderManagerClass(const ShaderManagerClass&);
    ~ShaderManagerClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();

    bool RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
    TextureShaderClass* m_TextureShader;
};

#endif