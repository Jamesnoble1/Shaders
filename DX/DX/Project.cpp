#include "Project.h"

Project::Project(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in) : BaseApplication(hinstance, hwnd, screenWidth, screenHeight, in)
{
	lockMovement = true;
	isDithering = false;
	isMenu = true;
	Light* tempLight;

	//set up main directional light
	tempLight = new Light;
	tempLight->SetAmbientColour(0.5f, 0.50f, 0.5f, 1.0f);
	tempLight->SetDiffuseColour(0.5f, 0.5f, 0.5f, 1.0f);
	tempLight->SetPosition(1.0f, 1.0f, 1.0f);
	tempLight->SetDirection(1.0f, -1.0f, 1.0f);
	lights.push_back(tempLight);

	//spotlight one
	tempLight = new Light;
	tempLight->SetAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	tempLight->SetDiffuseColour(1.0f, 0.0f, 0.0f, 0.5f);
	tempLight->SetPosition(80.0f, 10.0f, 80.0f);
	tempLight->SetDirection(1.0f, -1.0f, 1.0f);
	lights.push_back(tempLight);
	
	//spotlight two
	tempLight = new Light;
	tempLight->SetAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	tempLight->SetDiffuseColour(0.5f, 0.5f, 0.5f, 0.5f);
	tempLight->SetPosition(50.0f, 1.0f, 10.0f);
	tempLight->SetDirection(1.0f, -1.0f, 1.0f);
	lights.push_back(tempLight);
	
	renderTexture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	postProcess = new OrthoMesh(m_Direct3D->GetDevice(), screenWidth, screenHeight);
	menuMesh = new OrthoMesh(m_Direct3D->GetDevice(), 300, 300, 0, 0);
	ditherTest = new DitheringShader(m_Direct3D->GetDevice(), hwnd);
	renderMenu = new TextureShader(m_Direct3D->GetDevice(), hwnd);
	menuTexture = new Texture(m_Direct3D->GetDevice(), L"../res/menu.png");

	terrainManager = new TerrainManager(m_Direct3D, hwnd);
	
	//generate grassy by default
	terrainManager->Generate(GRASSY);

}
Project::~Project()
{
	
	delete ditherTest;
	delete postProcess;
	delete renderTexture;
	delete menuMesh;
	delete renderMenu;
	delete menuTexture;
	for (int i = 0; i < lights.size(); i++)
	{
		delete lights.at(i);
	}
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
}



bool Project::Frame()
{
	bool result;

	// Update the system stats.
	m_Timer->Frame();

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->isKeyDown(VK_ESCAPE) == true)
	{
		return false;
	}

	// Do the frame input processing.
	HandleInput(m_Timer->GetTime());


	// Render the graphics.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Project::Render()
{
	
	//matrices used by most functions, declared here as tend to not align properly when created at class instantiation
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseVeiwMatrix, orthoMatrix;
	// Clear the scene. 
	m_Direct3D->BeginScene(0.2f, 0.2f, 0.8f, 1.0f);

	if (lockMovement)
	{
		terrainManager->setCameraPos(m_Camera);
	}
	
	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseVeiwMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	
	if (isDithering)
	{
		renderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
		renderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.2, 0.2, 0.8, 1.0);
		m_Camera->Update();
		//terrain
		terrainManager->render(worldMatrix, viewMatrix, projectionMatrix, lights);
		m_Direct3D->GetWorldMatrix(worldMatrix);
		m_Direct3D->SetBackBufferRenderTarget();

		postProcess->SendData(m_Direct3D->GetDeviceContext());
		ditherTest->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, renderTexture->GetShaderResourceView());
		ditherTest->Render(m_Direct3D->GetDeviceContext(), postProcess->GetIndexCount());
	}
	else
	{
		terrainManager->render(worldMatrix, viewMatrix, projectionMatrix, lights);
		
	}
	
	if (isMenu)
	{
		menuMesh->SendData(m_Direct3D->GetDeviceContext());
		renderMenu->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, menuTexture->GetTexture());
		renderMenu->Render(m_Direct3D->GetDeviceContext(), menuMesh->GetIndexCount());
	}
	
	m_Direct3D->EndScene();
	return true;
}


void Project::HandleInput(float frameTime)
{
// Set the frame time for calculating the updated position.
m_Camera->SetFrameTime(frameTime);


	// Handle the input.
	if (m_Input->isKeyDown('L'))
	{
		if (lockMovement)
		{
			lockMovement = false;
		}
		else
		{
			lockMovement = true;
		}
		m_Input->SetKeyUp('L');
	}
	if (m_Input->isKeyDown('W'))
	{
		// forward
		m_Camera->MoveForward();
	}
	if (m_Input->isKeyDown('S'))
	{
		// back
		m_Camera->MoveBackward();
	}
	if (m_Input->isKeyDown('A'))
	{
		// Strafe Left
		m_Camera->StrafeLeft();
	}
	if (m_Input->isKeyDown('D'))
	{
		// Strafe Right
		m_Camera->StrafeRight();
	}
	if (m_Input->isKeyDown('Q') && !lockMovement)
	{
		// Down
		m_Camera->MoveDownward();
	}
	if (m_Input->isKeyDown('E') && !lockMovement)
	{
		// Up
		m_Camera->MoveUpward();
	}
	if (m_Input->isKeyDown(VK_UP))
	{
		// rotate up
		m_Camera->TurnUp();
	}
	if (m_Input->isKeyDown(VK_DOWN))
	{
		// rotate down
		m_Camera->TurnDown();
	}
	if (m_Input->isKeyDown(VK_LEFT))
	{
		// rotate left
		m_Camera->TurnLeft();
	}
	if (m_Input->isKeyDown(VK_RIGHT))
	{
		// rotate right
		m_Camera->TurnRight();
	}

	//turns on wireframe
	if (m_Input->isKeyDown('H'))
	{
		m_Direct3D->TurnOnWireframe();
	}

	
	//turns on wireframe
	if (m_Input->isKeyDown('J'))
	{
		m_Direct3D->TurnOffWireframe();
	}
	
	//turns mneu on and off
	if (m_Input->isKeyDown('M'))
	{
		if (isMenu)
		{
			isMenu = false;
		}
		else
		{
			isMenu = true;
		}
		m_Input->SetKeyUp('M');
	}

	//regenrate terrain
	if (m_Input->isKeyDown('R'))
	{
		terrainManager->reGenerate();
		m_Input->SetKeyUp('R');
	}

	if (m_Input->isKeyDown('1'))
	{
		terrainManager->Generate(GRASSY);
		m_Input->SetKeyUp('1');
	}
	if (m_Input->isKeyDown('2'))
	{
		terrainManager->Generate(SEASIDE);
		m_Input->SetKeyUp('2');
	}
	if (m_Input->isKeyDown('3'))
	{
		terrainManager->Generate(MOUNTAIN);
		m_Input->SetKeyUp('3');
	}
	if (m_Input->isKeyDown('4'))
	{
		terrainManager->Generate(DESERT);
		m_Input->SetKeyUp('4');
	}
	if (m_Input->isKeyDown('5'))
	{
		terrainManager->Generate(ROCKY);
		m_Input->SetKeyUp('5');
	}
	if (m_Input->isKeyDown('6'))
	{
		terrainManager->Generate(ISLAND);
		m_Input->SetKeyUp('6');
	}
	if (m_Input->isKeyDown(VK_TAB))
	{ 
		if (isDithering)
		{
			isDithering = false;
		}
		else
		{
			isDithering = true;
		}
	
		m_Input->SetKeyUp(VK_TAB);
	}

//alt mouse controls for camera, small offset to avoid jittering with small mousemovements
	if (m_Input->getMouseX() < (screenRect.right / 2) - MOUSE_DEADZONE)
	{
		m_Camera->TurnLeft();
	}

	if (m_Input->getMouseX() > (screenRect.right / 2) + MOUSE_DEADZONE)
	{
		m_Camera->TurnRight();
	}

	if (m_Input->getMouseY() < (screenRect.bottom / 2) - MOUSE_DEADZONE)
	{
		m_Camera->TurnUp();
	}

	if (m_Input->getMouseY() > (screenRect.bottom / 2) + MOUSE_DEADZONE)
	{
		m_Camera->TurnDown();
	}

		
	SetCursorPos(windowRect.right - screenRect.right/2, windowRect.bottom - screenRect.bottom /2);
}


//calculating if intersectoin occurs using method from Moller and Trumbore 2003
bool Project::triRayIntesection(XMFLOAT3 vertOne, XMFLOAT3 vertTwo, XMFLOAT3 vertThree, XMFLOAT3 camPos, XMFLOAT3 camVector)
{
	XMFLOAT3 pVector, qVector;
	float determinate, inverseDet, t, u, v;
	//make two vectors to describe triangle
	XMFLOAT3 vectorOne = XMFLOAT3(vertTwo.x - vertOne.x, vertTwo.y - vertOne.y, vertTwo.z - vertOne.z);
	XMFLOAT3 vectorTwo = XMFLOAT3(vertThree.x - vertOne.x, vertThree.y - vertOne.y, vertThree.z - vertOne.z);
	XMFLOAT3 tVector = XMFLOAT3(0.0f - vertOne.x, 0.0f - vertOne.y, 0.0 - vertOne.z);

	//take cross product of cam vector and vector two to store qVector
	crossProduct(&pVector, &camVector, &vectorTwo);
	
	//calculate the determinate
	dotProduct(&determinate, &pVector, &vectorOne);

	//check if det is 0. if 0 ray is in the same plane
	if (determinate > 0.000001 && determinate < 0.000001)
	{
		return false;
	}

	//take cross product of tVector and vectorOne
	crossProduct(&qVector, &tVector, &vectorOne);

	//calculate inverse det
	inverseDet = 1 / determinate;

	//calculate U
	dotProduct(&u, &pVector, &tVector);

	u *= inverseDet;

	//check if u is between 0 and 1. if not return false
	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	//calculate v
	dotProduct(&v, &qVector, &camVector);

	v *= inverseDet;

	//check if v is between 0 and 1 and that u + v <= 1 if not return false
	if (v < 0.0f || v > 1.0f || (u + v) > 1.0f)
	{
		return false;
	}

	//calculate t
	dotProduct(&t, &qVector, &vectorTwo);

	t *= inverseDet;

	//if t > 0 return true, else return false
	if (t > 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}

	
	
}

vector<XMFLOAT3> Project::findCollisionTris(XMFLOAT3 camPos, XMFLOAT3 camVector)
{
	vector<XMFLOAT3> triVerts;

//	triVerts.push_back(XMFLOAT3((int)camPos.x, collisionMesh[(int)camPos.x][(int)camPos.z], (int)camPos.z));
	return triVerts;
}
void Project::crossProduct(XMFLOAT3 * result, XMFLOAT3 * vecOne, XMFLOAT3 * vecTwo)
{
	result->x = (vecOne->y * vecTwo->z) - (vecOne->z * vecTwo->y);
	result->y = (vecOne->z * vecTwo->x) - (vecOne->x * vecTwo->z);
	result->z = (vecOne->x * vecTwo->y) - (vecOne->y * vecTwo->x);
}

void Project::dotProduct(float* result, XMFLOAT3 * vecOne, XMFLOAT3 * vecTwo)
{
	*result = vecOne->x * vecTwo->x + vecOne->y * vecTwo->y + vecOne->z * vecTwo->z;
}
