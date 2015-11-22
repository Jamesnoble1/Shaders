// Tessellation Mesh, point list for tessellation
#ifndef _TESSELLATIONMESH_H_
#define _TESSELLATIONMESH_H_

#include "BaseMesh.h"

using namespace DirectX;

class TessellationMesh : public BaseMesh
{

public:
	TessellationMesh(ID3D11Device* device, WCHAR* textureFilename, WCHAR* displacementFileName);
	~TessellationMesh();

	void SendData(ID3D11DeviceContext*);
	ID3D11ShaderResourceView* getDisplacement();
protected:
	void InitBuffers(ID3D11Device* device);
	void LoadDisplacement(ID3D11Device* device, WCHAR* filename);
	Texture* Displacement;
};

#endif