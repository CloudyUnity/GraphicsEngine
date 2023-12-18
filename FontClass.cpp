#include "FontClass.h"

FontClass::FontClass()
{
    m_Font = 0;
    m_Texture = 0;
}


FontClass::FontClass(const FontClass& other)
{
}


FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int fontChoice)
{
    char fontFilename[128];
    char fontTextureFilename[128];
    bool result;

    switch (fontChoice)
    {
    case 0:
    default:
    {
        strcpy_s(fontFilename, "../GraphicsEngine/Data/FontData.txt");
        strcpy_s(fontTextureFilename, "../GraphicsEngine/Data/Font4.tga");
        m_fontHeight = 32.0f;
        m_spaceSize = 3;
        break;
    }
    }

    return  LoadFontData(fontFilename) && LoadTexture(device, deviceContext, fontTextureFilename);
}

void FontClass::Shutdown()
{
    ReleaseTexture();
    ReleaseFontData();
}

bool FontClass::LoadFontData(char* filename)
{
    ifstream fin;
    char temp;

    // Create the font spacing buffer.
    m_Font = new FontType[95];

    // Read in the font size and spacing between chars.
    fin.open(filename);
    if (fin.fail())
        return false;

    // Read in the 95 used ascii characters for text.
    for (int i = 0; i < 95; i++)
    {
        fin.get(temp);
        while (temp != ' ')
            fin.get(temp);

        fin.get(temp);
        while (temp != ' ')
            fin.get(temp);

        fin >> m_Font[i].left;
        fin >> m_Font[i].right;
        fin >> m_Font[i].size;
    }

    fin.close();

    return true;
}

void FontClass::ReleaseFontData()
{
    if (m_Font)
    {
        delete[] m_Font;
        m_Font = 0;
    }
}

bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
    m_Texture = new TextureClass;
    return m_Texture->Initialize(device, deviceContext, filename);
}

void FontClass::ReleaseTexture()
{
    if (m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
    return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
    VertexType* vertexPtr;
    int numLetters, index = 0, letter;

    // Coerce the input vertices into a VertexType structure.
    vertexPtr = (VertexType*)vertices;

    numLetters = (int)strlen(sentence);

    // Draw each letter onto a quad.
    for (int i = 0; i < numLetters; i++)
    {
        letter = ((int)sentence[i]) - 32;

        // If the letter is a space then just move over three pixels.
        if (letter == 0)
        {
            drawX += m_spaceSize;
            continue;
        }
        
        // First triangle in quad.
        vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
        vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
        index++;

        vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // Bottom right.
        vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
        index++;

        vertexPtr[index].position = XMFLOAT3(drawX, (drawY - m_fontHeight), 0.0f);  // Bottom left.
        vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
        index++;

        // Second triangle in quad.
        vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
        vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
        index++;

        vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
        vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
        index++;

        vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // Bottom right.
        vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
        index++;

        // Update the x location for drawing by the size of the letter and one pixel.
        drawX = drawX + m_Font[letter].size + 1.0f;
    }
}

int FontClass::GetSentencePixelLength(char* sentence)
{
    int pixelLength = 0, numLetters, letter;

    numLetters = (int)strlen(sentence);

    for (int i = 0; i < numLetters; i++)
    {
        letter = ((int)sentence[i]) - 32;

        // If the letter is a space then count it as three pixels.
        if (letter == 0)
        {
            pixelLength += m_spaceSize;
            continue;
        }
        
        pixelLength += (m_Font[letter].size + 1);
    }

    return pixelLength;
}

int FontClass::GetFontHeight()
{
    return (int)m_fontHeight;
}