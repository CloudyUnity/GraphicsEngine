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

bool TextureSetClass::Add(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* texFileName, int setIndex)
{
	TextureClass tex;
	bool result = tex.Initialize(device, deviceContext, const_cast<char*>(texFileName));
	if (!result)
		return false;

	if (setIndex > 0 && setIndex < m_Textures.size())
	{
		m_Textures.at(setIndex) = tex.GetTexture();
		return true;
	}

	m_Textures.push_back(tex.GetTexture());
	return true;
}

bool TextureSetClass::AddCubemap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* texFileName, int setIndex)
{
	TextureClass tex;
	bool result = tex.InitializeCubemap(device, deviceContext, const_cast<char*>(texFileName));
	if (!result)
		return false;

	if (setIndex > 0 && setIndex < m_Textures.size())
	{
		m_Textures.at(setIndex) = tex.GetTexture();
		return true;
	}

	m_Textures.push_back(tex.GetTexture());
	return true;
}

bool TextureSetClass::Add(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<const char*> texFileName, int setIndex)
{
	TextureClass tex;

	vector<char*> newVector;
	for (auto c : texFileName)
		newVector.push_back(const_cast<char*>(c));

	bool result = tex.InitializeCubemap(device, deviceContext, newVector);
	if (!result)
		return false;

	if (setIndex > 0 && setIndex < m_Textures.size())
	{
		m_Textures.at(setIndex) = tex.GetTexture();
		return true;
	}

	m_Textures.push_back(tex.GetTexture());
	return true;
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
	return (int)m_Textures.size();
}

ID3D11ShaderResourceView* TextureSetClass::GetTexture(int i)
{
	return m_Textures.at(i);
}
