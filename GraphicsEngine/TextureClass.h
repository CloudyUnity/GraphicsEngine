#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include <stdio.h>
#include <vector>
#include "IShutdown.h"
using std::vector;

class TextureClass : public IShutdown
{
private:
    struct TargaHeader
    {
        unsigned char data1[12];
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char data2;
    };

public:
    TextureClass();
    ~TextureClass();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
    bool Initialize(ID3D11ShaderResourceView* srv);
    bool InitializeCubemap(ID3D11Device*, ID3D11DeviceContext*, vector<char*>);
    bool InitializeCubemap(ID3D11Device*, ID3D11DeviceContext*, char*);
    void Shutdown() override;

    ID3D11ShaderResourceView* GetSRV();

    int GetWidth();
    int GetHeight();

private:
    bool LoadTarga(char*, bool flip = false);

private:
    unsigned char* m_targaData;
    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;
    ID3D11ShaderResourceView* m_textureViews[6];
    int m_width, m_height;

    bool m_dontOwnSRV;
};

#endif