// Application.h
#ifndef PROJECT_H
#define PROJECT_H

#include "baseapplication.h"
#include "LightShader.h"
#include "Light.h"
#include "PlaneMesh.h"
#include "Terrain.h"
#include "TerrainGenerator.h"
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
	TerrainGenerator * terrainGenerator;
	Terrain * test, * testTwo;
	LightShader * testShader;
	PlaneMesh * testicles;
	void HandleInput(float frameTime);
	vector<Light*> lights;
	bool triRayIntesection(XMFLOAT3 vertOne, XMFLOAT3 vertTwo, XMFLOAT3 vertThree, XMFLOAT3 camPos, XMFLOAT3 forwardVector);
	
};

#endif