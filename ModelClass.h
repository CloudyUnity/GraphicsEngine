#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include "textureclass.h"
#include <fstream>
#include "LightClass.h"
using namespace DirectX;
using std::ifstream;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	struct VectorType
	{
		float x, y, z;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void RenderBuffers(ID3D11DeviceContext*);
	void Shutdown();	

	int GetIndexCount();
	float GetBoundingRadius();	

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();		
	bool LoadModel(char*);
	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);	

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;	
	ModelType* m_model;
	float m_boundingRadius;
};

#endif