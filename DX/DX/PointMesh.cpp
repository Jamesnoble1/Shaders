// Mesh.cpp
#include "pointmesh.h"

PointMesh::PointMesh(ID3D11Device* device, WCHAR* textureFilename, int triPoints)
{
	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	InitBuffers(device, triPoints);

	// Load the texture for this model.
	LoadTexture(device, textureFilename);
}


PointMesh::~PointMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}


void PointMesh::InitBuffers(ID3D11Device* device)
{
}

void PointMesh::InitBuffers(ID3D11Device* device, int triPoints)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set the number of vertices in the vertex array.
	m_vertexCount = triPoints;

	// Set the number of indices in the index array.
	m_indexCount = triPoints;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];

	// Load the vertex array with data.
	for (int i = 0; i < triPoints; i += 3)
	{
		vertices[i].position = XMFLOAT3(i, 1.0f, 0.0f);  // Top.
		vertices[i].texture = XMFLOAT2(0.0f, 1.0f);
		vertices[i].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		vertices[i + 1].position = XMFLOAT3(i, 1.3f, 0.0f);  // Bottom left.
		vertices[i + 1].texture = XMFLOAT2(0.0f, 0.0f);
		vertices[i + 1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		vertices[i + 2].position = XMFLOAT3(i, 1.6f, 0.0f);  // Bottom right.
		vertices[i + 2].texture = XMFLOAT2(1.0f, 0.0f);
		vertices[i + 2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	}
	

	// Load the index array with data.
	for (int i = 0; i < triPoints; i++)
	{
		indices[i] = i;
	}


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

void PointMesh::SendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

