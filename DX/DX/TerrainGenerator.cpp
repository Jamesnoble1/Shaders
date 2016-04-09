#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int sizeN, float smoothingMod)
{
	//sets class member variables
	m_sizeN = sizeN;
	m_resolution = pow(2, sizeN) + 1;
	m_smoothingMod = smoothingMod;
	smoothingMultiplier = pow(2, -smoothingMod);
	//create array
	terrainArray = new float *[m_resolution];
	for (int i = 0; i < m_resolution; i++)
	{
		terrainArray[i] = new float[m_resolution];
	}

	//seeds rand
	srand(std::chrono::steady_clock::now().time_since_epoch().count());
}

TerrainGenerator::~TerrainGenerator()
{
	//delete the terrain array
	for (int i = 0; i < m_resolution; i++)
	{
		delete terrainArray[i];
	}

	delete terrainArray;
}

float ** TerrainGenerator::getTerrain()
{
	return terrainArray;
}


int TerrainGenerator::getArraySize()
{
	return m_resolution;
}

//main function to calculate the terrain
void TerrainGenerator::calculateTerrain(float topLeft, float topRight, float bottomLeft, float bottomRight)
{
	//initialise/clear array
	for (int i = 0; i < m_resolution; i++)
	{
		for (int j = 0; j < m_resolution; j++)
		{
			terrainArray[i][j] = -10.0f;
		}
	}

	//set initial corners
	terrainArray[0][0] = bottomLeft;
	terrainArray[0][m_resolution - 1] = topLeft;
	terrainArray[(m_resolution - 1)][0] = bottomRight;
	terrainArray[(m_resolution - 1)][(m_resolution - 1)] = topRight;


	smoothingMultiplier = pow(2, -m_smoothingMod);

	//stride is distance between points being calculated in each direction. As array is "square" will be same for both axis
	//initial stride is integer division of m_sizeN
	int stride = m_resolution / 2;

	//default loc of 0, 0
	Location loc;

	//do initial triangle step
	squarePoints.push_back(loc);

	//stride halfed after each loop, as integer division will == 0 when complete 
	while (stride != 0)
	{
		//loops until all points have been popped from squarePoints vector
		while (squarePoints.size() != 0)
		{
			//triangle step for each squarePoint
			triangleStep(stride, squarePoints.back());
			squarePoints.pop_back();
		}

		//loops until all points have been popped from diamondPoints vector
		while (diamondPoints.size() != 0)
		{
			//square step for each point in diamondPoints
			squareStep(stride, diamondPoints.back());
			diamondPoints.pop_back();
		}


		//half stride
		stride /= 2;

		//change smoothing multiplier
		smoothingMultiplier *= pow(2, -m_smoothingMod);
	}

	squarePoints.clear();
	diamondPoints.clear();
}

//does the square step using the points generated in triangle step as it's root
// calculate value for all four points stride distance away from rootLocation on x and y directions
//to do this take average of 4 points stride distance away from this point and add a small random varience
//if any points needed go off the end of the array wrap around and use far side values
//Only calculates value if no value has been calculated previously
void TerrainGenerator::squareStep(int stride, Location rootLocation)
{

	float pos = 0.0;


	//Checks point to the negative x
	if ((rootLocation.x - (stride * 2)) < 0 && terrainArray[rootLocation.x - stride][rootLocation.y] < -9.0f)
	{
		//points go off left side so take far right for average
		terrainArray[rootLocation.x - stride][rootLocation.y] = (terrainArray[rootLocation.x][rootLocation.y] +
			//terrainArray[(m_resolution - 1) - stride][rootLocation.y] +
			terrainArray[rootLocation.x - stride][rootLocation.y + stride] +
			terrainArray[rootLocation.x - stride][rootLocation.y - stride]) / 3 + randomOffset();
		pos = terrainArray[rootLocation.x - stride][rootLocation.y];
		//pushes to vector
		pushSquarePoints(rootLocation.x - stride, rootLocation.y);
	}
	else if (terrainArray[rootLocation.x - stride][rootLocation.y] < -9.0f)
	{
		//all points valid
		terrainArray[rootLocation.x - stride][rootLocation.y] = (terrainArray[rootLocation.x][rootLocation.y] +
			terrainArray[rootLocation.x - (stride * 2)][rootLocation.y] +
			terrainArray[rootLocation.x - stride][rootLocation.y + stride] +
			terrainArray[rootLocation.x - stride][rootLocation.y - stride]) / 4 + randomOffset();
		pos = terrainArray[rootLocation.x - stride][rootLocation.y];
		//pushes to vector
		pushSquarePoints(rootLocation.x - stride, rootLocation.y);
	}

	//checks point to the positive x
	if ((rootLocation.x + (stride * 2)) > (m_resolution - 1) && terrainArray[rootLocation.x + stride][rootLocation.y] < -9.0f)
	{
		//point goes off right side
		terrainArray[rootLocation.x + stride][rootLocation.y] = (terrainArray[rootLocation.x][rootLocation.y] +
			//terrainArray[stride][rootLocation.y] +
			terrainArray[rootLocation.x + stride][rootLocation.y + stride] +
			terrainArray[rootLocation.x + stride][rootLocation.y - stride]) / 3 + randomOffset();

		pos = terrainArray[rootLocation.x + stride][rootLocation.y];
		//pushes to vector
		pushSquarePoints(rootLocation.x + stride, rootLocation.y);
	}
	else if (terrainArray[rootLocation.x + stride][rootLocation.y] < -9.0f)
	{
		//all points valid
		terrainArray[rootLocation.x + stride][rootLocation.y] = (terrainArray[rootLocation.x][rootLocation.y] +
			terrainArray[rootLocation.x + (stride * 2)][rootLocation.y] +
			terrainArray[rootLocation.x + stride][rootLocation.y + stride] +
			terrainArray[rootLocation.x + stride][rootLocation.y - stride]) / 4 + randomOffset();
		pos = terrainArray[rootLocation.x + stride][rootLocation.y];
		//pushes to vector
		pushSquarePoints(rootLocation.x + stride, rootLocation.y);
	}


	//checks point stride away on negative y
	if ((rootLocation.y - (stride * 2)) < 0 && terrainArray[rootLocation.x][rootLocation.y - stride] < -9.0f)
	{
		//point goes off top
		terrainArray[rootLocation.x][rootLocation.y - stride] = (terrainArray[rootLocation.x][rootLocation.y] +
			//terrainArray[rootLocation.x][(m_resolution - 1) - stride] +
			terrainArray[rootLocation.x + stride][rootLocation.y - stride] +
			terrainArray[rootLocation.x - stride][rootLocation.y - stride]) / 3 + randomOffset();
		//pushes to vector
		pos =
			terrainArray[rootLocation.x][rootLocation.y - stride];
		pushSquarePoints(rootLocation.x, rootLocation.y - stride);
	}
	else if (terrainArray[rootLocation.x][rootLocation.y - stride] < -9.0f)
	{
		//all points valid
		terrainArray[rootLocation.x][rootLocation.y - stride] = (terrainArray[rootLocation.x][rootLocation.y] +
			terrainArray[rootLocation.x][rootLocation.y - (stride * 2)] +
			terrainArray[rootLocation.x + stride][rootLocation.y - stride] +
			terrainArray[rootLocation.x - stride][rootLocation.y - stride]) / 4 + randomOffset();
		//pushes to vector
		pos = terrainArray[rootLocation.x][rootLocation.y - stride];
		pushSquarePoints(rootLocation.x, rootLocation.y - stride);
	}

	//checks point on positive y
	if ((rootLocation.y + (stride * 2)) > (m_resolution - 1) && terrainArray[rootLocation.x][rootLocation.y + stride] < -9.0f)
	{
		//point goes off bottom of array
		terrainArray[rootLocation.x][rootLocation.y + stride] = (terrainArray[rootLocation.x][rootLocation.y] +
			//terrainArray[rootLocation.x][stride] +
			terrainArray[rootLocation.x - stride][rootLocation.y + stride] +
			terrainArray[rootLocation.x + stride][rootLocation.y + stride]) / 3 + randomOffset();
		//pushes to vector
		pos = terrainArray[rootLocation.x][rootLocation.y - stride];
		pushSquarePoints(rootLocation.x, rootLocation.y + stride);
	}
	else if (terrainArray[rootLocation.x][rootLocation.y + stride] < -9.0f)
	{
		//all points valid
		terrainArray[rootLocation.x][rootLocation.y + stride] = (terrainArray[rootLocation.x][rootLocation.y] +
			terrainArray[rootLocation.x][rootLocation.y + (stride * 2)] +
			terrainArray[rootLocation.x - stride][rootLocation.y + stride] +
			terrainArray[rootLocation.x + stride][rootLocation.y + stride]) / 4
			+ randomOffset();

		//pushes to vector
		pushSquarePoints(rootLocation.x, rootLocation.y + stride);
	}

}
float TerrainGenerator::randomOffset()
{
	
	return  ((float)(rand() % 50) - 25.0f) * smoothingMultiplier;
}
void TerrainGenerator::pushSquarePoints(int x, int y)
{
	//temp location to add to vector
	Location loc;

	//pushes a location to the vector
	loc.x = x;
	loc.y = y;
	squarePoints.push_back(loc);
}
//does the "triangle step" in calculating the displacemnet
//calculates the centre point for each diagonal created from square step
void TerrainGenerator::triangleStep(int stride, Location rootLocation)
{
	Location loc;
	float pos;

	//Bottom Right
	//check that point is a valid array location
	if ((rootLocation.x + stride) < m_resolution && (rootLocation.y + stride) < m_resolution)
	{
		//checks if point has been calculated previously
		if (terrainArray[rootLocation.x + stride][rootLocation.y + stride] < -9.0f)
		{
			//calculate point value using average of 4 corners + random value
			terrainArray[rootLocation.x + stride][rootLocation.y + stride] = (terrainArray[rootLocation.x][rootLocation.y] +
				terrainArray[rootLocation.x][rootLocation.y + (stride * 2)] +
				terrainArray[rootLocation.x + (stride * 2)][rootLocation.y + (stride * 2)] +
				terrainArray[rootLocation.x + (stride * 2)][rootLocation.y]) / 4 + randomOffset();

			loc.x = rootLocation.x + stride;
			loc.y = rootLocation.y + stride;
			diamondPoints.push_back(loc);
		}
	}

	//bottom left
	//check that point is a valid array location
	if ((rootLocation.x - stride) > 0 && (rootLocation.y + stride) < m_resolution)
	{
		//checks if point has been calculated previously
		if (terrainArray[rootLocation.x - stride][rootLocation.y + stride] < -9.0f)
		{
			//calculate point value using average of 4 corners + random value
			terrainArray[rootLocation.x - stride][rootLocation.y + stride] = (terrainArray[rootLocation.x][rootLocation.y] +
				terrainArray[rootLocation.x][rootLocation.y + (stride * 2)] +
				terrainArray[rootLocation.x - (stride * 2)][rootLocation.y + (stride * 2)] +
				terrainArray[rootLocation.x - (stride * 2)][rootLocation.y]) / 4 + randomOffset();


			loc.x = rootLocation.x - stride;
			loc.y = rootLocation.y + stride;
			diamondPoints.push_back(loc);
		}
	}

	//top right
	//check that point is a valid array location
	if ((rootLocation.x + stride) < m_resolution && (rootLocation.y - stride) > 0)
	{
		//checks if point has been calculated previously
		if (terrainArray[rootLocation.x + stride][rootLocation.y - stride] < -9.0f)
		{
			//calculate point value using average of 4 corners + random value
			terrainArray[rootLocation.x + stride][rootLocation.y - stride] = (terrainArray[rootLocation.x][rootLocation.y] +
				terrainArray[rootLocation.x][rootLocation.y - (stride * 2)] +
				terrainArray[rootLocation.x + (stride * 2)][rootLocation.y - (stride * 2)] +
				terrainArray[rootLocation.x + (stride * 2)][rootLocation.y]) / 4 + randomOffset();
			//+ (float)(rand() % 1);

			loc.x = rootLocation.x + stride;
			loc.y = rootLocation.y - stride;
			diamondPoints.push_back(loc);
		}

	}

	//top left
	//check that point is a valid array location
	if ((rootLocation.x - stride) > 0 && (rootLocation.y - stride) > 0)
	{
		//checks if point has been calculated previously
		if (terrainArray[rootLocation.x - stride][rootLocation.y - stride] < -9.0f)
		{
			//calculate point value using average of 4 corners + random value
			terrainArray[rootLocation.x - stride][rootLocation.y - stride] = (terrainArray[rootLocation.x][rootLocation.y] +
				terrainArray[rootLocation.x][rootLocation.y - (stride * 2)] +
				terrainArray[rootLocation.x - (stride * 2)][rootLocation.y + (stride * 2)] +
				terrainArray[rootLocation.x - (stride * 2)][rootLocation.y]) / 4 + randomOffset();
			//+ (float)(rand() % 1);

			loc.x = rootLocation.x - stride;
			loc.y = rootLocation.y - stride;
			diamondPoints.push_back(loc);
		}
	}
}

