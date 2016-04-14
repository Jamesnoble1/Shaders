// Light shader.h
// Basic single light shader setup
#ifndef _LIGHTSHADER_H_
#define _LIGHTSHADER_H_

#include "BaseShader.h"
#include "light.h"
#include <vector>

using namespace std;
using namespace DirectX;


class LightShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse[3];
		XMFLOAT4 ambient[3];
		XMFLOAT3 lightPositionOne;
		float paddingOne = 0.0f;
		XMFLOAT3 lightPositionTwo;
		float paddingTwo = 0.0f;
		XMFLOAT3 lightPositionThree;
		float paddingThree = 0.0f;
	};

	struct colourBufferType
	{
		XMFLOAT4 lightColours[16];
		XMFLOAT4 numValues;
		XMFLOAT4 lightCutoffs[16];
		
	};

public:

	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();
	
	//takes in world, view, projection matrices, a texture resource view, a light vector, float 4 with the cutoffs and pointer to an XMFLOAT4[4] for colours
	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection,
							  vector<Light*> light, float* cuttoffs, XMFLOAT4* colourValues, int numValues);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_colourBuffer;
};

#endif