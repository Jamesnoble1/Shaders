// texture shader.h
#ifndef COLOUR_FILTER_H
#define COLOUR_FILTER_H

#include "BaseShader.h"

using namespace std;
using namespace DirectX;


class ColourFilterShader : public BaseShader
{

public:

	ColourFilterShader(ID3D11Device* device, HWND hwnd, WCHAR* colourFilerPath);
	~ColourFilterShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif