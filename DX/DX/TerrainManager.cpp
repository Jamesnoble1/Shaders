#include "TerrainManager.h"

TerrainManager::TerrainManager(D3D * direct3D, HWND hwnd)
{

	//sets up member variables
	m_Direct3D = direct3D;
	currentSize = 0;

	//sets up shader class
	lightShader = new LightShader(direct3D->GetDevice(), hwnd);

	//setup colours. takes more memory but saves creating the array every time terrain generated
	//arrays to hold thresholds and colours
	grassColours = new XMFLOAT4[5];
	grassThresholds = new float[5];
	desertColours = new XMFLOAT4[4];
	desertThresholds = new float[4];
	mountainColours = new XMFLOAT4[8];
	mountainThresholds = new float[8];
	seaColours = new XMFLOAT4[5];
	seaThresholds = new float[5];
	rockyColours  = new XMFLOAT4[4];
	rockyThresholds = new float[4];
	islandColours = new XMFLOAT4[6];
	islandThresholds = new float[6];

	initialiseArrays();

}
TerrainManager::~TerrainManager()
{
	
	delete lightShader;
	delete terrain;
	delete generator;
	delete grassColours;
	delete grassThresholds;
	delete desertColours;
	delete desertThresholds;
	delete mountainColours;
	delete mountainThresholds;
	delete seaColours;
	delete seaThresholds;
	delete rockyColours;
	delete rockyThresholds;
	delete islandColours;
	delete islandThresholds;
}


//generates terain of the chosen type. needs to be called atleast once before render
void TerrainManager::Generate(TerrainType chosenTerrain)
{
	if (terrain != nullptr)
	{
		delete terrain;
		delete generator;
	}
	
	currentTerrain = chosenTerrain;

	//generates the terrain and creates the mesh
	switch (currentTerrain)
	{
	case GRASSY:
	{
				   grassSetup();
				   break;
	}
	case DESERT:
	{
				   desertSetup();
				   break;
	}
	case MOUNTAIN:
	{
					 mountainSetup();
					 break;
	}
	case SEASIDE:
	{
					seaSetup();
					break;
	}
	case ROCKY:
	{
				  rockySetup();
				  break;
	}
	case ISLAND:
	{
				   islandSetup();
				   break;
	}

	default:
	{
			   break;
	}
	}
}

//does the same as above but calls with current terrain
//and then calls Generate
void TerrainManager::reGenerate()
{
	Generate(currentTerrain);
}

//renders current terrain using matrices passed in
void TerrainManager::render(const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, vector<Light*>  lightVec)
{


	//always need to send mesh to graphics card
	terrain->SendData(m_Direct3D->GetDeviceContext());

	
	lightShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, lightVec, colourThresholds, colourArray, currentSize);
	lightShader->Render(m_Direct3D->GetDeviceContext(), terrain->GetIndexCount());

}

//sets the camera position
void TerrainManager::setCameraPos(Camera * camPtr)
{

	//quick and dirty test "collision" to be replaced with triangle ray intersection technique once finished
	//works ok as long as change in height isn't too large then camera will clip into terrain until it catches up
		float camPosX, camPosY, camPosZ;
		if (camPtr->GetPosition().x >= 0.0 && camPtr->GetPosition().x < arraySize)
		{
			camPosX = camPtr->GetPosition().x;
		}
		else if (camPtr->GetPosition().x < 0.0)
		{
			camPosX = 0.0f;
		}
		else
		{
			camPosX = arraySize - 1;
		}
		camPosY = camPtr->GetPosition().y;

		if (camPtr->GetPosition().z >= 0.0 && camPtr->GetPosition().z < arraySize)
		{
			camPosZ = camPtr->GetPosition().z;
		}
		else if (camPtr->GetPosition().z < 0.0)
		{
			camPosZ = 0.0f;
		}
		else
		{
			camPosZ = arraySize - 1;
		}

		//reduces bumps as navigating collision. 
		//works by only changing y pos by a fraction of the difference each frame  correct to within a certain range
		if (abs((collisionArray[(int)camPosX / 4][(int)camPosZ / 4] + 2.0f) - camPosY) > 0.1f)
		{
			camPosY += (collisionArray[(int)camPosX / 4][(int)camPosZ / 4] + 2.0f - camPosY) / 10;
		}

		camPtr->SetPosition(camPosX, camPosY, camPosZ);

	}


void TerrainManager::grassSetup()
{
	//generate terrain
	generator = new TerrainGenerator(8, 0.9);
	generator->calculateTerrain(2.0f, 3.5f, 3.0f, 2.5f);
	terrain = new Terrain(m_Direct3D->GetDevice(), generator->getTerrain(), generator->getArraySize());
	//set up collision array
	collisionArray = generator->getCollision();
	arraySize = generator->getArraySize();
	
	//set up colours
	colourArray = grassColours;
	colourThresholds = grassThresholds;
	currentSize = 5;
	
}
void TerrainManager::desertSetup()
{
	//generate terrain
	generator = new TerrainGenerator(8, 1.2);
	generator->calculateTerrain(2.0f, 3.5f, 3.0f, 2.5f);
	terrain = new Terrain(m_Direct3D->GetDevice(), generator->getTerrain(), generator->getArraySize());
	//set up collision array
	collisionArray = generator->getCollision();
	arraySize = generator->getArraySize();
	
	//set up colours
	colourArray = desertColours;
	colourThresholds = desertThresholds;
	currentSize = 4;
}
void TerrainManager::mountainSetup()
{
	//generate terrain
	generator = new TerrainGenerator(8, 0.8);
	generator->calculateTerrain(50.0f, 3.5f, 50.0f, 2.5f);
	terrain = new Terrain(m_Direct3D->GetDevice(), generator->getTerrain(), generator->getArraySize());
	//set up collision array
	collisionArray = generator->getCollision();
	arraySize = generator->getArraySize();
	
	//set up colours
	colourArray = mountainColours;
	colourThresholds = mountainThresholds;
	currentSize = 8;

}
void TerrainManager::seaSetup()
{
	//generate terrain
	generator = new TerrainGenerator(8, 1.2);
	generator->calculateTerrain(-10.0f, -10.5f, 5.0f, 4.5f);
	terrain = new Terrain(m_Direct3D->GetDevice(), generator->getTerrain(), generator->getArraySize());
	//set up collision array
	collisionArray = generator->getCollision();
	arraySize = generator->getArraySize();
	
	//set up colours
	colourArray = seaColours;
	colourThresholds = seaThresholds;
	currentSize = 5;
	
}
void TerrainManager::rockySetup()
{
	//generate terrain
	generator = new TerrainGenerator(8, 0.4);
	generator->calculateTerrain(10.0f, 7.0f, 4.0f, 7.0f);
	terrain = new Terrain(m_Direct3D->GetDevice(), generator->getTerrain(), generator->getArraySize());
	//set up collision array
	collisionArray = generator->getCollision();
	arraySize = generator->getArraySize();
	
	//set up colours
	colourArray = rockyColours;
	colourThresholds = rockyThresholds;
	currentSize = 4;
	
}
void TerrainManager::islandSetup()
{
	//generate terrain
	generator = new TerrainGenerator(8, 0.6);
	generator->calculateTerrain(0.5, -3.5f, 0.0f, 1.5f);
	terrain = new Terrain(m_Direct3D->GetDevice(), generator->getTerrain(), generator->getArraySize());

	//set up collision array
	collisionArray = generator->getCollision();
	arraySize = generator->getArraySize();
	

	//set up colours
	colourArray = islandColours;
	colourThresholds = islandThresholds;
	currentSize = 5;
	
}

//initialise array values. Used to keep constructor tidy
void TerrainManager::initialiseArrays()
{
	//initialise grass array contents
	grassColours[0] = XMFLOAT4(0.3f, 0.3f, 0.4f, 1.0f);
	grassColours[1] = XMFLOAT4(0.6f, 0.4f, 0.4f, 1.0f);
	grassColours[2] = XMFLOAT4(0.2f, 0.7f, 0.2f, 1.0f);
	grassColours[3] = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
	grassColours[4] = XMFLOAT4(0.01f, 0.01f, 1.0f, 1.0f);

	grassThresholds[0] = 10.0f;
	grassThresholds[1] = 5.0f;
	grassThresholds[2] = 2.0f;
	grassThresholds[3] = 0.0f;
	grassThresholds[4] = -5.0f;

	//initialise desert values
	desertColours[0] = XMFLOAT4(0.76f, 0.69f, 0.50f, 1.0f);
	desertColours[1] = XMFLOAT4(0.86f, 0.79f, 0.60f, 1.0f);
	desertColours[2] = XMFLOAT4(0.96f, 0.89f, 0.70f, 1.0f);
	desertColours[3] = XMFLOAT4(0.66f, 0.59f, 0.40f, 1.0f);

	desertThresholds[0] = 10.f;
	desertThresholds[1] = 5.0f;
	desertThresholds[2] = 0.0f;
	desertThresholds[3] = -5.0f;

	//initialise mountain values
	mountainColours[0] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mountainColours[1] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mountainColours[2] = XMFLOAT4(0.76f, 0.76f, 0.76f, 1.0f);
	mountainColours[3] = XMFLOAT4(0.6f, 0.4f, 0.4f, 1.0f);
	mountainColours[4] = XMFLOAT4(0.6f, 0.4f, 0.2f, 1.0f);
	mountainColours[5] = XMFLOAT4(0.1f, 0.69f, 0.1f, 1.0f);
	mountainColours[6] = XMFLOAT4(0.0f, 0.69f, 0.0f, 1.0f);
	mountainColours[7] = XMFLOAT4(0.1f, 0.1f, 0.9f, 1.0f);

	mountainThresholds[0] = 40.0f;
	mountainThresholds[1] = 30.0f;
	mountainThresholds[2] = 25.0f;
	mountainThresholds[3] = 20.0f;
	mountainThresholds[4] = 15.0f;
	mountainThresholds[5] = 10.0f;
	mountainThresholds[6] = 5.0f;
	mountainThresholds[7] = 1.0f;

	//initialise sea values
	seaColours[0] = XMFLOAT4(0.0f, 0.69f, 0.0f, 1.0f);
	seaColours[1] = XMFLOAT4(0.86f, 0.79f, 0.60f, 1.0f);
	seaColours[2] = XMFLOAT4(0.1f, 0.1f, 0.9f, 1.0f);
	seaColours[3] = XMFLOAT4(0.1f, 0.1f, 0.4f, 1.0f);
	seaColours[4] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	seaThresholds[0] = 10.0f;
	seaThresholds[1] = 0.0f;
	seaThresholds[2] = -2.0f;
	seaThresholds[3] = -5.0f;
	seaThresholds[4] = -10.0f;

	//initialise rocky colours
	rockyColours[0] = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	rockyColours[1] = XMFLOAT4(0.6f, 0.4f, 0.4f, 1.0f);
	rockyColours[2] = XMFLOAT4(0.6f, 0.3f, 0.2f, 1.0f);
	rockyColours[3] = XMFLOAT4(0.1f, 0.1f, 0.6f, 1.0f);

	rockyThresholds[0] = 15.0f;
	rockyThresholds[1] = 10.0f;
	rockyThresholds[2] = 5.0f;
	rockyThresholds[3] = -10.0f;

	//initialise island colours
	islandColours[0] = XMFLOAT4(0.1f, 0.69f, 0.1f, 1.0f);
	islandColours[1] = XMFLOAT4(0.86f, 0.79f, 0.60f, 1.0f);
	islandColours[2] = XMFLOAT4(0.96f, 0.89f, 0.70f, 1.0f);
	islandColours[3] = XMFLOAT4(0.1f, 0.1f, 0.9f, 1.0f);
	islandColours[4] = XMFLOAT4(0.1f, 0.1f, 0.4f, 1.0f);
	islandColours[5] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	islandThresholds[0] = 2.0f;
	islandThresholds[1] = 0.0f;
	islandThresholds[2] = -1.0f;
	islandThresholds[3] = -3.0f;
	islandThresholds[4] = -10.0f;
}