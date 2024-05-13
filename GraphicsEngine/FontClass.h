#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

#include <directxmath.h>
#include <fstream>
#include "IShutdown.h"

using namespace DirectX;
using std::ifstream;

#include "textureclass.h"

class FontClass : public IShutdown
{
private:
    struct FontType
    {
        float left, right;
        int size;
    };

    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    FontClass();
    FontClass(const FontClass&);
    ~FontClass();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int);
    void Shutdown() override;

    TextureClass* GetTexture();

    void BuildVertexArray(void*, char*, float, float);
    int GetSentencePixelLength(char*);
    int GetFontHeight();

private:
    bool LoadFontData(char*);
    void ReleaseFontData();
    bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
    void ReleaseTexture();

private:
    FontType* m_Font;
    TextureClass* m_Texture;
    float m_fontHeight;
    int m_spaceSize;
};

#endif