#include "Project.h"

Project::Project(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in) : BaseApplication(hinstance, hwnd, screenWidth, screenHeight, in)
{
	lockMovement = true;

	Light* tempLight;

	//set up main directional light
	tempLight = new Light;
	tempLight->SetAmbientColour(0.5f, 0.50f, 0.5f, 1.0f);
	tempLight->SetDiffuseColour(0.8f, 0.8f, 0.8f, 1.0f);
	tempLight->SetPosition(1.0f, 1.0f, 1.0f);
	tempLight->SetDirection(1.0f, -1.0f, 1.0f);
	lights.push_back(tempLight);

	//spotlight one
	tempLight = new Light;
	tempLight->SetAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	tempLight->SetDiffuseColour(0.5f, 0.5f, 0.5f, 0.5f);
	tempLight->SetPosition(1.0f, 1.0f, 1.0f);
	tempLight->SetDirection(1.0f, -1.0f, 1.0f);
	lights.push_back(tempLight);
	
	//spotlight two
	tempLight = new Light;
	tempLight->SetAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	tempLight->SetDiffuseColour(0.5f, 0.5f, 0.5f, 0.5f);
	tempLight->SetPosition(50.0f, 1.0f, 10.0f);
	tempLight->SetDirection(1.0f, -1.0f, 1.0f);
	lights.push_back(tempLight);
	
	terrainGenerator = new TerrainGenerator(8, 0.6f);
	terrainGenerator->calculateTerrain(5.0f, -3.0f, -5.0f, 2.0f); 
	test = new Terrain(m_Direct3D->GetDevice(), L"../res/bunny.png", terrainGenerator->getTerrain(), terrainGenerator->getArraySize());
	
	testShader = new LightShader(m_Direct3D->GetDevice(), hwnd);


}
Project::~Project()
{
	delete terrainGenerator;
	delete test;
	delete testShader;

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

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseVeiwMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	
	XMMATRIX tempWorld = worldMatrix;
	worldMatrix *= XMMatrixScaling(0.5f, 0.5f, 0.5f);
	//terrain
	test->SendData(m_Direct3D->GetDeviceContext());
	XMFLOAT4 testColours[4] = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.6f, 0.4f, 0.2f, 1.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f) , XMFLOAT4(0.01f, 0.01f, 1.0f, 1.0f) };
	XMFLOAT4 cutoffs = XMFLOAT4(20.0f, 5.0f, -10.0f, -15.0f);
	testShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, test->GetTexture(), lights, cutoffs, testColours);
	testShader->Render(m_Direct3D->GetDeviceContext(), test->GetIndexCount());
	//worldMatrix *= XMMatrixTranslation(25.5, 0, 0);
	

	

	worldMatrix = tempWorld;
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

bool Project::triRayIntesection(XMFLOAT3 vertOne, XMFLOAT3 vertTwo, XMFLOAT3 vertThree, XMFLOAT3 camPos, XMFLOAT3 forwardVector)
{
	//make two vectors to describe triangle
	XMFLOAT3 vectorOne = XMFLOAT3(vertTwo.x - vertOne.x, vertTwo.y - vertOne.y, vertTwo.z - vertOne.z);
	XMFLOAT3 vectorTwo = XMFLOAT3(vertThree.x - vertOne.x, vertThree.y - vertOne.y, vertThree.z - vertOne.z);
}
