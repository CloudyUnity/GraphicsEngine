#ifndef _shaderclass_H_
#define _shaderclass_H_

const int NUM_LIGHTS = 4;
const int SIN_COUNT = 32;

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

// XMMATRIX = 4x4 floats = 64 bytes

class ShaderClass : public IShutdown
{
public:
	struct CBufferType{};
	struct MatrixBufferType : CBufferType // 192 bytes
	{
		XMMATRIX world; 
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct UtilBufferType : CBufferType // 16 bytes
	{
		float time;
		float texelSizeX;
		float texelSizeY;
		float padding;
	};
	struct LightColorBufferType : CBufferType  // 16n bytes
	{
		XMFLOAT4 diffuseColor[NUM_LIGHTS];
	};
	struct LightPositionBufferType : CBufferType // 16n bytes
	{
		XMFLOAT4 lightPosition[NUM_LIGHTS];
	};
	struct LightBufferType : CBufferType  // 64 bytes
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;

		XMFLOAT3 lightDirection;
		float specularPower;

		XMFLOAT4 specularColor;
	};
	struct CameraBufferType : CBufferType  // 16 bytes
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
	struct PixelBufferType : CBufferType // 16 bytes
	{
		XMFLOAT4 pixelColor;
	};
	struct FogBufferType : CBufferType // 16 bytes
	{
		float fogStart;
		float fogEnd;

		float pad1, pad2;
	};
	struct ClipPlaneBufferType : CBufferType  // 16 bytes
	{
		XMFLOAT4 clipPlane;
	};
	struct TexTranslationBufferType : CBufferType  // 16 bytes
	{
		XMFLOAT2 translation;
		float timeMultiplier;
		float pad;
	};
	struct AlphaBufferType : CBufferType  // 16 bytes
	{
		float alphaBlend;
		float pad1, pad2, pad3;
	};
	struct ReflectionBufferType : CBufferType  // 64 bytes
	{
		XMMATRIX reflectionMatrix;
	};
	struct WaterBufferType : CBufferType  // 16 bytes
	{
		float reflectRefractScale;
		XMFLOAT3 padding;
	};
	struct FireBufferType : CBufferType // 32 bytes
	{
		XMFLOAT2 distortion1, distortion2, distortion3;
		float distortionScale, distortionBias;
	};
	struct ShadowBufferType : CBufferType
	{
		XMMATRIX shadowView, shadowProj; 
		float usingShadows, poissonSpread, shadowBias, shadowCutOff;
		XMFLOAT4 poissonDisk[NUM_POISSON_SAMPLES];		
	};
	struct BlurBufferType : CBufferType // 16 + 16n bytes 
	{
		float blurMode;

		XMFLOAT3 pad3;

		XMFLOAT4 weights[BLUR_SAMPLE_SPREAD];
	};
	struct FilterBufferType : CBufferType
	{
		BOOL grainEnabled, monochromeEnabled, sharpnessEnabled, chromaticEnabled, vignetteEnabled;

		float vignetteStrength, vignetteSmoothness, padding;

		float sharpnessKernalN, sharpnessKernalP, sharpnessStrength, grainIntensity;
	};
	struct TessellationBufferType : CBufferType
	{
		float tessellationAmount;
		XMFLOAT3 padding;
	};
	struct OceanSineBufferType : CBufferType
	{
		XMFLOAT4 ampPhaseFreq[SIN_COUNT];
	};

public:
	struct ShaderParamsGlobalType
	{		
		UtilBufferType utils;
		LightColorBufferType lightColor;
		LightPositionBufferType lightPos;
		LightBufferType light;
		CameraBufferType camera;
		FogBufferType fog;		
		ReflectionBufferType reflection;		
		ShadowBufferType shadow;		
		OceanSineBufferType oceanSine;

		bool reflectionEnabled;
	};
	struct ShaderParamsObjectType
	{
		MatrixBufferType matrix;
		PixelBufferType pixel;
		ClipPlaneBufferType clip;
		TexTranslationBufferType textureTranslation;
		AlphaBufferType alpha;
		WaterBufferType water;
		FireBufferType fire;
		TessellationBufferType tesselation;		

		// PP
		BlurBufferType blur;
		FilterBufferType filter;
	};

public:
	ShaderClass();
	~ShaderClass();

	bool Initialize(ID3D11Device*, HWND, char*, char*, bool clampSamplerMode = false);
	void Shutdown() override;
	bool Render(ID3D11DeviceContext*, int, TextureSetClass*, ShaderParamsGlobalType*, ShaderParamsObjectType*);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, bool);	
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	
	virtual bool SetShaderParameters(ID3D11DeviceContext*, TextureSetClass*, ShaderParamsGlobalType*, ShaderParamsObjectType*);	
	virtual void RenderShader(ID3D11DeviceContext*, int);

	bool SaveCBufferInfo(ID3D10Blob*, ID3D10Blob*);
	virtual bool UsesCBuffer(string);
	int UsesCBufferVertex(string);
	int UsesCBufferFragment(string);

private:	
	bool TryCreateBuffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer*& ptr, size_t structSize, string);
	void UnmapVertexBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer);
	void UnmapFragmentBuffer(ID3D11DeviceContext* deviceContext, int bufferNumber, ID3D11Buffer** buffer);

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer, * m_utilBuffer, * m_lightColorBuffer, * m_lightPositionBuffer, * m_lightBuffer, * m_cameraBuffer, * m_pixelBuffer, * m_fogBuffer;
	ID3D11Buffer* m_clipBuffer, * m_texTransBuffer, * m_alphaBuffer, * m_reflectionBuffer, * m_waterBuffer, * m_fireBuffer, * m_shadowBuffer;
	ID3D11Buffer* m_blurBuffer, * m_filterBuffer, * m_tesselationBuffer, * m_oceanSineBuffer;
	ID3D11SamplerState* m_sampleState;

	string m_vertexName, m_fragName; // TO BE REMOVED
	vector<string> m_cbufferListVertex;
	vector<string> m_cbufferListFragment;

	vector<ID3D11Buffer*> m_bufferList;

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

private:

	template<typename T>
	bool SetShaderCBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, T values, string name)
	{
		int bufferIndex;

		bufferIndex = UsesCBufferVertex(name);
		if (bufferIndex != -1)
		{
			T* ptr;
			if (!TryMapBuffer(deviceContext, &buffer, &ptr))
				return false;
			*ptr = values;
			UnmapVertexBuffer(deviceContext, bufferIndex, &buffer);
		}

		bufferIndex = UsesCBufferFragment(name);
		if (bufferIndex != -1)
		{
			T* ptr;
			if (!TryMapBuffer(deviceContext, &buffer, &ptr))
				return false;
			*ptr = values;
			UnmapFragmentBuffer(deviceContext, bufferIndex, &buffer);
		}
	}
};

#endif
