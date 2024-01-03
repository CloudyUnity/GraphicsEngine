#ifndef _TEXTURESHADERCLASS_H_
#define _TEXTURESHADERCLASS_H_

const int NUM_LIGHTS = 4;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "LightClass.h"
#include "TextureClass.h"
#include <unordered_map>
#include <any>
#include "TextureSetClass.h"
#include "Constants.h"
using namespace DirectX;
using std::ofstream;
using std::string;
using std::unordered_map;
using std::any;
using std::any_cast;

class ShaderClass
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
	struct FogBufferType 
	{
		float fogStart;
		float fogEnd;
		float pad1, pad2;
	};
	struct ClipPlaneBufferType
	{
		XMFLOAT4 clipPlane;
	};
	struct TexTranslationBufferType 
	{
		XMFLOAT2 translation;
		float timeMultiplier;
		float pad;
	};
	struct AlphaBufferType
	{
		float alphaBlend;
		float pad1, pad2, pad3;
	};
	struct ReflectionBufferType
	{
		XMMATRIX reflectionMatrix;
	};

public:
	ShaderClass();
	ShaderClass(const ShaderClass&);
	~ShaderClass();

	bool Initialize(ID3D11Device*, HWND, char*, char*);
	void Shutdown();	
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, TextureSetClass*, unordered_map<string, any> = {});

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool TryCreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer** ptr, size_t structSize, string, string);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, TextureSetClass*, unordered_map<string, any>);
	void RenderShader(ID3D11DeviceContext*, int);

	bool ShaderUsesBuffer(std::string, std::string);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer, * m_utilBuffer, * m_lightColorBuffer, * m_lightPositionBuffer, * m_lightBuffer, * m_cameraBuffer, * m_pixelBuffer, * m_fogBuffer;
	ID3D11Buffer* m_clipBuffer, * m_texTransBuffer, * m_alphaBuffer, * m_reflectionBuffer;
	ID3D11SamplerState* m_sampleState;

	std::string m_vertexName, m_fragName;
};

#endif
