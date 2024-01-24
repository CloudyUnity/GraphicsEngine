#ifndef _TEXTURESETCLASS_H_
#define _TEXTURESETCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include "TextureClass.h"
#include <vector>
#include <string>
using namespace DirectX;
using std::vector;
using std::string;

class TextureSetClass
{
public:
	TextureSetClass();
	TextureSetClass(const TextureSetClass&);
	~TextureSetClass();

	void Shutdown();
	void ReleaseTexture();
	bool Add(ID3D11Device*, ID3D11DeviceContext*, const char*, int setIndex = -1);
	bool AddCubemap(ID3D11Device*, ID3D11DeviceContext*, const char*, int setIndex = -1);
	bool Add(ID3D11Device*, ID3D11DeviceContext*, vector<const char*>, int setIndex = -1);
	void Add(ID3D11ShaderResourceView*, int setIndex = -1);
	void ClearList();

	int GetCount();
	ID3D11ShaderResourceView* GetTexture(int);

private:
	vector<ID3D11ShaderResourceView*> m_Textures;	
};

#endif