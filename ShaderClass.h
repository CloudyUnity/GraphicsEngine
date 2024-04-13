#ifndef _shaderclass_H_
#define _shaderclass_H_

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
#include "settings.h"
#include "IShutdown.h"

using namespace DirectX;
using std::ofstream;
using std::string;
using std::unordered_map;
using std::any;
using std::any_cast;
using std::unique_ptr;

class ShaderClass : public IShutdown
{
public:
	struct BufferType {};

	struct CBufferType
	{
		ID3D11Buffer* BufferPtr;
		BufferType BufferValues;
		BufferType* MappedPtr;

		CBufferType(BufferType values)
		{
			BufferPtr = nullptr;
			BufferValues = values;
			MappedPtr = nullptr;
		}
	};	

	// XMMATRIX = 4x4 floats = 64 bytes
	struct MatrixBufferType : public BufferType // 192 bytes
	{
		XMMATRIX world; 
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct UtilBufferType : public BufferType // 16 bytes
	{
		float time;
		float texelSizeX;
		float texelSizeY;
		float padding;
	};
	struct LightColorBufferType : public BufferType // 16n bytes
	{
		XMFLOAT4 diffuseColor[NUM_LIGHTS];
	};
	struct LightPositionBufferType : public BufferType // 16n bytes
	{
		XMFLOAT4 lightPosition[NUM_LIGHTS];
	};
	struct LightBufferType : public BufferType // 64 bytes
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;

		XMFLOAT3 lightDirection;
		float specularPower;

		XMFLOAT4 specularColor;
	};
	struct CameraBufferType : public BufferType // 16 bytes
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
	struct PixelBufferType : public BufferType // 16 bytes
	{
		XMFLOAT4 pixelColor;
	};
	struct FogBufferType : public BufferType // 16 bytes
	{
		float fogStart;
		float fogEnd;

		float pad1, pad2;
	};
	struct ClipPlaneBufferType : public BufferType // 16 bytes
	{
		XMFLOAT4 clipPlane;
	};
	struct TexTranslationBufferType : public BufferType // 16 bytes
	{
		XMFLOAT2 translation;
		float timeMultiplier;
		float pad;
	};
	struct AlphaBufferType : public BufferType // 16 bytes
	{
		float alphaBlend;
		float pad1, pad2, pad3;
	};
	struct ReflectionBufferType : public BufferType // 64 bytes
	{
		XMMATRIX reflectionMatrix;
	};
	struct WaterBufferType : public BufferType // 16 bytes
	{
		float reflectRefractScale;
		XMFLOAT3 padding;
	};
	struct FireBufferType : public BufferType // 32 bytes
	{
		XMFLOAT2 distortion1, distortion2, distortion3;
		float distortionScale, distortionBias;
	};
	struct ShadowBufferType : public BufferType
	{
		XMMATRIX shadowView, shadowProj; 
		float usingShadows, poissonSpread, shadowBias, shadowCutOff;
		XMFLOAT4 poissonDisk[NUM_POISSON_SAMPLES];		
	};
	struct BlurBufferType : public BufferType // 16 + 16n bytes 
	{
		float blurMode;

		XMFLOAT3 pad3;

		XMFLOAT4 weights[BLUR_SAMPLE_SPREAD];
	};
	struct FilterBufferType : public BufferType
	{
		BOOL grainEnabled, monochromeEnabled, sharpnessEnabled, chromaticEnabled, vignetteEnabled;

		float vignetteStrength, vignetteSmoothness, padding;

		float sharpnessKernalN, sharpnessKernalP, sharpnessStrength, grainIntensity;
	};

public:
	ShaderClass();
	~ShaderClass();

	virtual bool Initialize(ID3D11Device*, HWND);
	void Shutdown() override;
	bool Render(ID3D11DeviceContext*, int, TextureSetClass*);

protected:
	bool InitializeShader(ID3D11Device*, HWND, const char*, const char*, bool, bool);	
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	virtual bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*);
	void RenderShader(ID3D11DeviceContext*, int);
	void UnmapVertexBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer);
	void UnmapFragmentBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer);
	bool TryCreateBuffer(ID3D11Device* device, BufferType);

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;	
	ID3D11SamplerState* m_sampleState;

	vector<ID3D11Buffer*> m_bufferList;

	D3D11_BUFFER_DESC m_bufferDesc;

	vector<CBufferType> m_cbufferList;

	template <typename T>
	bool TryMapBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer** buffer, T** outPtr)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;		

		HRESULT result = deviceContext->Map(*buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
			return false;

		*outPtr = (T*)mappedResource.pData;

		return true;
	}	
};

#endif
