#ifndef _TERRAINGENERATOR_H_
#define _TERRAINGENERATOR_H_

//location struct to hold array locations to perform the steps from
struct Location
{
	int x = 0;
	int y = 0;
};

#include <math.h>
#include <vector>
#include <chrono>

using std::vector;


class TerrainGenerator
{
public:
	TerrainGenerator(int sizeN, float smoothingMod);
	~TerrainGenerator();
	void calculateTerrain(float topLeft, float topRight, float bottomLeft, float bottomRight);
	float ** getTerrain();
	float ** getCollision();
	void setTilable(bool tile);
	int getArraySize();

private:
	int m_resolution;
	int m_sizeN;
	float m_smoothingMod;
	float smoothingMultiplier;
	bool isTilable;
	float ** terrainArray;
	float ** collisionArray;
	void triangleStep(int stride, Location rootLocation);
	void squareStep(int stride, Location rootLocation);
	void fillCollisionArray();
	vector<Location> diamondPoints, squarePoints;
	float randomOffset();
	void pushSquarePoints(int x, int y);
};



#endif