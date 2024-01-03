#include "TextureSetClass.h"

TextureSetClass::TextureSetClass()
{
}

TextureSetClass::TextureSetClass(const TextureSetClass&)
{
}

TextureSetClass::~TextureSetClass()
{
}

void TextureSetClass::Shutdown()
{
	ReleaseTexture();
}

void TextureSetClass::Add(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* texFileName, int setIndex)
{
	TextureClass tex;
	tex.Initialize(device, deviceContext, const_cast<char*>(texFileName));

	if (setIndex > 0 && setIndex < m_Textures.size())
	{
		m_Textures.at(setIndex) = tex.GetTexture();
		return;
	}

	m_Textures.push_back(tex.GetTexture());
}

void TextureSetClass::Add(ID3D11ShaderResourceView* tex, int setIndex)
{
	if (setIndex >= 0 && setIndex < m_Textures.size())
	{
		m_Textures.at(setIndex) = tex;
		return;
	}

	m_Textures.push_back(tex);
}

void TextureSetClass::ReleaseTexture()
{
	
}

void TextureSetClass::ClearList()
{
	m_Textures.clear();
}

int TextureSetClass::GetCount()
{
	return m_Textures.size();
}

ID3D11ShaderResourceView* TextureSetClass::GetTexture(int i)
{
	return m_Textures.at(i);
}
