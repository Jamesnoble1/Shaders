// Dithering
#ifndef CHROMA_KEY_H
#define CHROMA_KEY_H

#include "BaseShader.h"

using namespace std;
using namespace DirectX;


class DitheringShader : public BaseShader
{

public:

	DitheringShader(ID3D11Device* device, HWND hwnd);
	~DitheringShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif