#include "ModelLineListClass.h"

bool ModelLineListClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<XMFLOAT3> points)
{
	m_isModelLineList = true;

	m_points = points;

	m_boundingRadius = 0;
	for (int i = 0; i < points.size(); i++)
	{
		float dist = sqrt(points[i].x * points[i].x + points[i].y * points[i].y + points[i].z * points[i].z);
		if (dist > m_boundingRadius)
			m_boundingRadius = dist;
	}

    return InitializeBuffers(device);
}

void ModelLineListClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(LineVertexType);
	offset = 0;

	// Set the vertex/index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

bool ModelLineListClass::InitializeBuffers(ID3D11Device* device)
{	
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = 50;
	m_indexCount = m_vertexCount;

	m_vertices = new LineVertexType[m_vertexCount];
	memset(m_vertices, 0, (sizeof(LineVertexType) * m_vertexCount));

	indices = new unsigned long[m_indexCount];

	for (int i = 0; i < m_vertexCount; i++)
	{
		if (i < m_points.size())
			m_vertices[i].position = m_points[i];

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(LineVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	delete[] indices;
	indices = 0;

	return true;
}

bool ModelLineListClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, vector<XMFLOAT3> points)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LineVertexType* verticesPtr;

	m_points = points;
	m_vertexCount = points.size();
	m_indexCount = m_vertexCount;

	m_boundingRadius = 0;
	for (int i = 0; i < points.size(); i++)
	{
		float dist = sqrt(points[i].x * points[i].x + points[i].y * points[i].y + points[i].z * points[i].z);
		if (dist > m_boundingRadius)
			m_boundingRadius = dist;
	}

	memset(m_vertices, 0, (sizeof(LineVertexType) * m_vertexCount));

	for (int i = 0; i < m_points.size(); i++)
	{
		m_vertices[i].position = m_points[i];
	}

	HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	verticesPtr = (LineVertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)m_vertices, (sizeof(LineVertexType) * m_vertexCount));
	deviceContext->Unmap(m_vertexBuffer, 0);
}