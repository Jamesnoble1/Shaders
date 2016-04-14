//Terain mesh
//uses midpoint displacement to create a terrain surface
//Procedural terrain calculated on CPU then sent to GPU
//based on Dr Paul Roebertson's PlaneMesh.cpp
#include "Terrain.h"

Terrain::Terrain(ID3D11Device* device,  float ** terrainArray, int arraySize)
{
	//m_arraySzie is the size of the array to store into the buffer
	m_arraySize = arraySize;
	
	//sets m_terrainArray to point to array storing data
	m_terrainArray = terrainArray;

	// Initialize the vertex and index buffer that hold the geometry Terrain.
	InitBuffers(device);

	
	
}

Terrain::~Terrain()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
	
	
}

//calculates a unit length normal vector for ligting calculations. 
//Does this by getting the cross product of two of the vectors that make up a side of the tri and then normalising
XMFLOAT3 Terrain::calculateNormal(XMFLOAT3 vertOne, XMFLOAT3 vertTwo, XMFLOAT3 vertThree)
{
	//result to hold vector
	XMFLOAT3 result;

	//holds length to turn it into a unit vector
	float vecLength = 1.0f;

	//calculate vectors used for calculation
	XMFLOAT3 vecOne = XMFLOAT3(vertThree.x - vertOne.x, vertThree.y - vertOne.y, vertThree.z - vertOne.z);
		XMFLOAT3 vecTwo =  XMFLOAT3(vertTwo.x - vertOne.x, vertTwo.y - vertOne.y, vertTwo.z - vertOne.z);

	//take cross product
	result.x = (vecOne.y * vecTwo.z) - (vecOne.z * vecTwo.y);
	result.y = (vecOne.z * vecTwo.x) - (vecOne.x * vecTwo.z);
	result.z = (vecOne.x * vecTwo.y) - (vecOne.y * vecTwo.x);

	//finds lenght of result vector
	vecLength = sqrtf(result.x * result.x + result.y * result.y + result.z * result.z);

	result.x /= vecLength;
	result.y /= vecLength;
	result.z /= vecLength;

	return result;

}
void Terrain::InitBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index;
	XMFLOAT3 tempNormal;
	float  u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = m_arraySize * m_arraySize * 8;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	//current position in the vertices array
	
	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];

	// Initialize the index to the vertex array.
	index = 0;

	// UV coords.
	u = 0;
	v = 0;
	increment = 1.0f / m_arraySize;

	//convert terrainarray to vertex array
	

	for (int j = 0; j < (m_arraySize - 1); j++)
	{
		//each itteration of inner loop does two quads made from four tris
		//Then shifts to the next set of two quads along the x axis
		//Three columns and two rows used. Columns Left, middle and right. Rows Bottom and top
		for (int i = 1; i < (m_arraySize - 1); i += 2)
		{
			
			//tri one
			//top left
			vertices[index].position = XMFLOAT3((float)(i -1), m_terrainArray[(i - 1)][j], (float)j);
			vertices[index].texture = XMFLOAT2(u, v);
			indices[index] = index;
			index++; //increment vertex number

			u += increment;

			//centre top
			vertices[index].position = XMFLOAT3((float)i, m_terrainArray[i][j], (float)j);
			vertices[index].texture = XMFLOAT2(u, v);
			indices[index] = index;
			index++; //increment vertex number

			//bottom left
			vertices[index].position = XMFLOAT3((float)(i - 1), m_terrainArray[(i - 1)][(j + 1)], (float)(j + 1));
			vertices[index].texture = XMFLOAT2(u - increment, v + increment);
			indices[index] = index;
			

			//gets normal for this tri
			tempNormal = calculateNormal(vertices[(index - 2)].position, vertices[(index - 1)].position, vertices[index].position);
			
			//sets normals
			vertices[index].normal = tempNormal;
			vertices[(index - 1)].normal = tempNormal;
			vertices[(index - 2)].normal = tempNormal;
			
			index++; //increment vertex number
			
			//tri two
			//bottom left
			vertices[index].position = XMFLOAT3((float)(i - 1), m_terrainArray[(i - 1)][(j + 1)], (float)(j + 1));
			vertices[index].texture = XMFLOAT2((u - increment), v + increment);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++; //increment vertex number

			//centre top
			vertices[index].position = XMFLOAT3((float)i, m_terrainArray[i][j], (float)j);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++; //increment vertex number

			//centre bottom
			vertices[index].position = XMFLOAT3((float)i, m_terrainArray[i][(j + 1)], (float)(j + 1));
			vertices[index].texture = XMFLOAT2(u, (v + increment));
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;

			//gets normal for this tri
			tempNormal = calculateNormal(vertices[(index - 2)].position, vertices[(index - 1)].position, vertices[index].position);

			//sets normals
			vertices[index].normal = tempNormal;
			vertices[(index - 1)].normal = tempNormal;
			vertices[(index - 2)].normal = tempNormal;

			index++; //increment vertex number
			
			//tri three
			//centre top
			vertices[index].position = XMFLOAT3((float)i, m_terrainArray[i][j], (float)j);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++; //increment vertex number

			//right bottom
			vertices[index].position = XMFLOAT3((float)(i + 1), m_terrainArray[(i + 1)][(j + 1)], (float)(j + 1));
			vertices[index].texture = XMFLOAT2((u + increment), (v + increment));
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++; //increment vertex number

			//centre bottom
			vertices[index].position = XMFLOAT3((float)i, m_terrainArray[i][(j + 1)], (float)(j + 1));
			vertices[index].texture = XMFLOAT2(u, (v + increment));
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;

			//gets normal for this tri
			tempNormal = calculateNormal(vertices[(index - 2)].position, vertices[(index - 1)].position, vertices[index].position);

			//sets normals
			vertices[index].normal = tempNormal;
			vertices[(index - 1)].normal = tempNormal;
			vertices[(index - 2)].normal = tempNormal;

			index++; //increment vertex number
		
			//tri  four
			//centre top
			vertices[index].position = XMFLOAT3((float)i, m_terrainArray[i][j], (float)j);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++; //increment vertex number
			
			//increase u 
			u += increment;

			//right top
			vertices[index].position = XMFLOAT3((float)(i + 1), m_terrainArray[(i + 1)][j], (float)j);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++;

			//right bottom
			vertices[index].position = XMFLOAT3((float)(i + 1), m_terrainArray[(i + 1)][(j + 1)], (float)(j + 1));
			vertices[index].texture = XMFLOAT2(u , (v + increment));
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;

			//gets normal for this tri
			tempNormal = calculateNormal(vertices[(index - 2)].position, vertices[(index - 1)].position, vertices[index].position);

			//sets normals
			vertices[index].normal = tempNormal;
			vertices[(index - 1)].normal = tempNormal;
			vertices[(index - 2)].normal = tempNormal;

			index++;
			
		
		}
		u = 0;
		v += increment;
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

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}


