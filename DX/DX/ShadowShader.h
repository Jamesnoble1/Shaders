// Light shader.h
// Basic single light shader setup
//Currently not working
#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_

#include "BaseShader.h"
#include "light.h"
#include <vector>

using namespace std;
using namespace DirectX;


class ShadowShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[3];
		XMMATRIX lightProjection[3];
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[3];
		XMFLOAT4 diffuse[3];
	};

	struct LightBufferType2
	{
		XMFLOAT3 positionOne;
		float paddingOne = 0.0f;
		XMFLOAT3 positionTwo;
		float paddingTwo = 0.0f;
		XMFLOAT3 positionThree;
		float paddingThree = 0.0f;
	};

public:

	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, 
								ID3D11ShaderResourceView* depthMapOne, ID3D11ShaderResourceView* depthMapTwo, ID3D11ShaderResourceView* depthMapThree, vector<Light*> light);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_LightBuffer;
	ID3D11Buffer* m_LightBuffer2;

};

#endif