#ifndef _ORTHOMESH_H_
#define _ORTHOMESH_H_

#include "basemesh.h"

using namespace DirectX;

class OrthoMesh : public BaseMesh
{

public:
	OrthoMesh(ID3D11Device* device, int width, int height, WCHAR* textureFilename, int xPosition = 0, int yPosition = 0); //for specifying texture
	OrthoMesh(ID3D11Device* device, int width, int height, int xPosition = 0, int yPosition = 0); //for render to texture orthos
	~OrthoMesh();

protected:
	void InitBuffers(ID3D11Device*);
	int m_width, m_height, m_xPosition, m_yPosition;
};

#endif