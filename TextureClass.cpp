#include "textureclass.h"

TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;

	m_height = 0;
	m_width = 0;
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	HRESULT hResult;	

	result = LoadTarga(filename);
	if (!result)
		return false;

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
		return false;

	unsigned int rowPitch = (m_width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
		return false;

	deviceContext->GenerateMips(m_textureView);

	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

bool TextureClass::InitializeCubemap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<char*> filenames)
{
	bool result;
	HRESULT hResult;

	ID3D11Texture2D* m_textures[6];

	if (filenames.size() != 6)
		return false;

	// Load each face of the cubemap
	for (int i = 0; i < 6; ++i)
	{
		result = LoadTarga(filenames.at(i));
		if (!result)
			return false;

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Height = m_height;
		textureDesc.Width = m_width;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		// Create the empty texture for each face
		hResult = device->CreateTexture2D(&textureDesc, NULL, &m_textures[i]);
		if (FAILED(hResult))
			return false;

		unsigned int rowPitch = (m_width * 4) * sizeof(unsigned char);

		// Copy the targa image data into the texture for each face
		deviceContext->UpdateSubresource(m_textures[i], 0, NULL, m_targaData, rowPitch, 0);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		hResult = device->CreateShaderResourceView(m_textures[i], &srvDesc, &m_textureViews[i]);
		if (FAILED(hResult))
			return false;

		delete[] m_targaData;
		m_targaData = 0;
	}

	// Create the cubemap from the six faces
	D3D11_TEXTURE2D_DESC cubemapDesc = {};
	cubemapDesc.Width = m_width;
	cubemapDesc.Height = m_height;	
	cubemapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cubemapDesc.SampleDesc.Count = 1;
	cubemapDesc.SampleDesc.Quality = 0;
	cubemapDesc.Usage = D3D11_USAGE_DEFAULT;
	cubemapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	cubemapDesc.CPUAccessFlags = 0;

	cubemapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	cubemapDesc.MipLevels = 4; // CHOSEN RANDOMLY. 1-3 also works
	cubemapDesc.ArraySize = 6;  // Number of faces in the cubemap

	hResult = device->CreateTexture2D(&cubemapDesc, NULL, &m_texture);
	if (FAILED(hResult))
		return false;

	// Copy each face into the cubemap
	for (int i = 0; i < 6; ++i)
	{
		deviceContext->CopySubresourceRegion(m_texture, D3D11CalcSubresource(0, i, cubemapDesc.MipLevels),
			0, 0, 0, m_textures[i], 0, NULL);
	}

	// Create the shader resource view for the cubemap
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubemapDesc.Format;	
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = cubemapDesc.MipLevels;

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
		return false;

	deviceContext->GenerateMips(m_textureView);

	// delete[] m_textures;

	/*for (int i = 0; i < 6; ++i)
	{
		if (m_textures[i])
			m_textures[i]->Release();
	}*/

	return true;
}

bool TextureClass::InitializeCubemap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	HRESULT hResult;

	result = LoadTarga(filename, true);
	if (!result)
		return false;

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = m_height / 3;
	textureDesc.Width = m_width / 4;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA initData[6];
	for (int i = 0; i < 6; ++i)
	{
		int xOffset, yOffset;

		if (i == 0) { xOffset = m_width / 2; yOffset = m_height / 3; }		   // +X
		else if (i == 1) { xOffset = 0; yOffset = m_height / 3; }		   // -X
		else if (i == 2) { xOffset = m_width / 4; yOffset = 0; }				   // +Y
		else if (i == 3) { xOffset = m_width / 4; yOffset = 2 * (m_height / 3); }  // -Y
		else if (i == 4) { xOffset = m_width / 4; yOffset = m_height / 3; }		   // +Z
		else { xOffset = 3 * (m_width / 4); yOffset = m_height / 3; }		   // -Z

		initData[i].pSysMem = &m_targaData[(yOffset * m_width + xOffset) * 4];
		initData[i].SysMemPitch = m_width * 4;
		initData[i].SysMemSlicePitch = 0;
	}

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, initData, &m_texture);
	if (FAILED(hResult))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
		return false;

	// deviceContext->GenerateMips(m_textureView);

	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	if (m_textureViews)
	{
		/*for (int i = 0; i < 6; i++)
			m_textureViews[i]->Release();*/

		// delete[] m_textureViews;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

bool TextureClass::LoadTarga(char* filename, bool flipUpsideDown)
{
	int error, imageSize, index;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
		return false;

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
		return false;

	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	int bytesPerPixel = bpp / 8;
	imageSize = m_width * m_height * 4;

	targaImage = new unsigned char[imageSize];
	fread(targaImage, 1, imageSize, filePtr);	

	error = fclose(filePtr);
	if (error != 0)
		return false;

	m_targaData = new unsigned char[imageSize];
	index = 0;	

	int k = (m_width * m_height * bytesPerPixel) - (m_width * bytesPerPixel);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
	for (int j = 0; j < m_height; j++)
	{
		for (int i = 0; i < m_width; i++)
		{
			int s = flipUpsideDown ? (j * m_width + i) * bytesPerPixel : k;

			m_targaData[index + 0] = targaImage[s + 2];  // Red.
			m_targaData[index + 1] = targaImage[s + 1];  // Green.
			m_targaData[index + 2] = targaImage[s + 0];  // Blue

			if (bytesPerPixel == 4)
				m_targaData[index + 3] = targaImage[s + 3];  // Alpha
			else
				m_targaData[index + 3] = 255;  // Alpha

			// Increment the indexes into the targa data.
			k += bytesPerPixel;
			index += 4;
		}

		k -= (m_width * bytesPerPixel * 2);
	}

	delete[] targaImage;
	targaImage = 0;

	return true;
}

int TextureClass::GetWidth()
{
	return m_width;
}


int TextureClass::GetHeight()
{
	return m_height;
}