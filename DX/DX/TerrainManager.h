//terrain manager to handle the set up and display of generated terrains
#ifndef TERRAIN_MANAGER_H
#define TERRIAN_MANAGER_H

#include "TerrainGenerator.h"
#include "Terrain.h"
#include "LightShader.h"
#include "Camera.h"
#include "d3d.h"
#include "Light.h"
#include <vector>

using std::vector;


enum TerrainType
{
	GRASSY,
	DESERT,
	MOUNTAIN,
	SEASIDE,
	ROCKY, 
	ISLAND
};

class TerrainManager
{
public:

	TerrainManager(D3D * direct3D, HWND hwnd);
	~TerrainManager();

	//generates terain of the chosen type
	void Generate(TerrainType chosenTerrain);
	
	//does the same as above but makes sure any memory allocated with new is cleared
	//and then calls Generate
	void reGenerate();

	//renders current terrain using matrices passed in
	void render(const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, vector<Light*>  lightVec);

	//sets the camera position
	void setCameraPos(Camera * camPtr);
private:

	TerrainGenerator *generator;
	Terrain *terrain;
	LightShader *lightShader;
	float ** collisionArray;
	XMFLOAT4 *colourArray;
	int arraySize;
	float *colourThresholds;
	int currentSize;

	D3D * m_Direct3D;

	TerrainType currentTerrain;

	//set up terrain generator and then return a pointer to it
	void grassSetup();
	void desertSetup();
	void mountainSetup();
	void seaSetup();
	void rockySetup();
	void islandSetup();

	//initialise arrays.
	void initialiseArrays();
	//arrays to hold thresholds and colours
	XMFLOAT4 *grassColours;
	float * grassThresholds;
	XMFLOAT4 *desertColours;
	float * desertThresholds;
	XMFLOAT4 *mountainColours;
	float * mountainThresholds;
	XMFLOAT4 *seaColours;
	float * seaThresholds;
	XMFLOAT4 *rockyColours;
	float * rockyThresholds;
	XMFLOAT4 *islandColours;
	float * islandThresholds;
};

#endif