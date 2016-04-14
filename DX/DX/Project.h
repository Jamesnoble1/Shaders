// Application.h
#ifndef PROJECT_H
#define PROJECT_H

#include "baseapplication.h"

#include "Light.h"
#include "Texture.h"
#include "TerrainManager.h"
#include "RenderTexture.h"
#include "DitheringShader.h"
#include "TextureShader.h"
#include "OrthoMesh.h"
#include <vector>

using std::vector;

class Project : public BaseApplication
{
public:

	Project(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
	~Project();


	bool Frame();

private:
	bool Render();
	bool lockMovement;
	bool isDithering;
	bool isMenu;
	OrthoMesh  * postProcess;
	OrthoMesh * menuMesh;
	TerrainManager* terrainManager;
	DitheringShader * ditherTest;
	RenderTexture * renderTexture;
	TextureShader * renderMenu;
	Texture * menuTexture;
	void HandleInput(float frameTime);
	vector<Light*> lights;
	bool triRayIntesection(XMFLOAT3 vertOne, XMFLOAT3 vertTwo, XMFLOAT3 vertThree, XMFLOAT3 camPos, XMFLOAT3 camVector);
	vector<XMFLOAT3> findCollisionTris(XMFLOAT3 camPos, XMFLOAT3 camVector);
	void crossProduct(XMFLOAT3 * result, XMFLOAT3 * vecOne, XMFLOAT3 * vecTwo);
	void dotProduct(float* result, XMFLOAT3 * vecOne, XMFLOAT3 * vecTwo);

	
};

#endif