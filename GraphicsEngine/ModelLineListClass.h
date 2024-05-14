#ifndef _MODELLINELISTCLASS_H_
#define _MODELLINELISTCLASS_H_

#include "ModelClass.h"

class ModelLineListClass : public ModelClass
{
private:
	struct LineVertexType
	{
		XMFLOAT3 position;
	};

public:
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, vector<XMFLOAT3>);
	void RenderBuffers(ID3D11DeviceContext*) override;
	bool UpdateBuffers(ID3D11DeviceContext*, vector<XMFLOAT3>);

private:
	bool InitializeBuffers(ID3D11Device*) override;

	vector<XMFLOAT3> m_points;
	LineVertexType* m_vertices;
};

#endif