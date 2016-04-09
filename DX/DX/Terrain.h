#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "BaseMesh.h"

#include <math.h>
#include <vector>

using std::vector;



class Terrain : public BaseMesh
{

public:
	
	Terrain(ID3D11Device* device, WCHAR* textureFilename, float ** terrainArray, int arraySize);
	~Terrain();
	

protected:
	void InitBuffers(ID3D11Device* device);
	XMFLOAT3 calculateNormal(XMFLOAT3 vertOne, XMFLOAT3 vertTwo, XMFLOAT3 vertThree);
	float** m_terrainArray;
	int m_arraySize;
	

};

#endif