#ifndef _TEXTURESHADERCLASS_H_
#define _TEXTURESHADERCLASS_H_

const int NUM_LIGHTS = 4;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <chrono>
using namespace DirectX;
using std::ofstream;

class TextureShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct UtilBufferType 
	{
		float time, pad2, pad3;
		bool is2D, pad1, pad4, pad5;
	};
	struct LightColorBufferType
	{
		XMFLOAT4 diffuseColor[NUM_LIGHTS];
	};
	struct LightPositionBufferType
	{
		XMFLOAT4 lightPosition[NUM_LIGHTS];
	};
	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float specularPower;
		XMFLOAT4 specularColor;
	};
	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
	struct PixelBufferType
	{
		XMFLOAT4 pixelColor;
	};

public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4[], XMFLOAT4[], XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float, ID3D11ShaderResourceView* tex2 = nullptr, ID3D11ShaderResourceView* alphaMap = nullptr, ID3D11ShaderResourceView* normal = nullptr);
	bool RenderFont(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4[], XMFLOAT4[], XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float, ID3D11ShaderResourceView* tex2 = nullptr, ID3D11ShaderResourceView* alphaMap = nullptr, ID3D11ShaderResourceView* normal = nullptr);
	bool SetShaderParametersFont(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);
	void RenderShaderFont(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader, *m_fontPixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer, * m_utilBuffer, * m_lightColorBuffer, * m_lightPositionBuffer, * m_lightBuffer, * m_cameraBuffer, * m_pixelBuffer;
	ID3D11SamplerState* m_sampleState;
	std::chrono::high_resolution_clock::time_point m_startTime;
};

#endif
