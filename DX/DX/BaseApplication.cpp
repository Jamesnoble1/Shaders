// BaseApplication.cpp
// Base application functionality for inheritnace.
#include "BaseApplication.h"


BaseApplication::BaseApplication(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	m_Input = in;

	// Create the Direct3D object.
	m_Direct3D = new D3D(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	// Initialize the Direct3D object.
	if (!m_Direct3D)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		exit(EXIT_FAILURE);
	}

	// Create the camera object.
	m_Camera = new Camera();
	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Update();
	
	// Create the timer object.
	m_Timer = new Timer();

	//sets initial state
	currentState = START;
	currentFilter = NONE;

	//defaults show options to false
	showOptions = false;

	//sets post processing off
	shouldBlur = false;
	isHighBlur = false;
	//used to see if blur state has changed
	blurUpdated = false;
	//sets default for tesselation factor
	tesselationFactor = 1;
	

	//gets window rect
	GetWindowRect(hwnd, &windowRect);
	GetClientRect(hwnd, &screenRect);

	SetCursorPos(windowRect.right - screenRect.right / 2, windowRect.bottom - screenRect.bottom / 2);
}


BaseApplication::~BaseApplication()
{
	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool BaseApplication::Frame()
{
	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->isKeyDown(VK_ESCAPE) == true)
	{
		return false;
	}

	// Update the system stats.
	m_Timer->Frame();

	// Do the frame input processing.
	HandleInput(m_Timer->GetTime());

	return true;
}


void BaseApplication::HandleInput(float frameTime)
{
	// Set the frame time for calculating the updated position.
	m_Camera->SetFrameTime(frameTime);

	//cycles between colour filters
	if (m_Input->isKeyDown(VK_TAB))
	{
		switch (currentFilter)
		{
		case NONE:
			currentFilter = GREYSCALE;
			m_Input->SetKeyUp(VK_TAB);
			break;
		case GREYSCALE:
			currentFilter = NO_RED;
			m_Input->SetKeyUp(VK_TAB);
			break;
		case NO_RED:
			currentFilter = NO_GREEN;
			m_Input->SetKeyUp(VK_TAB);
			break;
		case NO_GREEN:
			currentFilter = NO_BLUE;
			m_Input->SetKeyUp(VK_TAB);
			break;
		case NO_BLUE:
			currentFilter = NONE;
			m_Input->SetKeyUp(VK_TAB);
			break;
		default:
			break;
		}
		
	}

	//turns menu on or off
	if (m_Input->isKeyDown('M'))
	{
		if (showOptions)
		{
			showOptions = false;
		}
		else
		{
			showOptions = true;
		}
		m_Input->SetKeyUp('M');
	}

	//turns blur on or off
	if (m_Input->isKeyDown('B'))
	{
		if (shouldBlur && isHighBlur)
		{
			shouldBlur = false;
			isHighBlur = false;
			blurUpdated = true;
		}
		else if (shouldBlur)
		{
			isHighBlur = true;
			blurUpdated = true;
		}
		else
		{
			blurUpdated = true;
			shouldBlur = true;
		}
		m_Input->SetKeyUp('B');
	}
	//changes game state
	if (m_Input->isKeyDown('1'))
	{
		// Mountains
		currentState = MOUNTAINS;
		setCamera(MOUNTAINS);
	}

	if (m_Input->isKeyDown('2'))
	{
		// Mountains
		currentState = LIGHTING;
		setCamera(LIGHTING);
	}
	
	// Handle the input.
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
	if (m_Input->isKeyDown('Q'))
	{
		// Down
		m_Camera->MoveDownward();
	}
	if (m_Input->isKeyDown('E'))
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
	//controlls for tesselation factor
	if (m_Input->isKeyDown('Z'))
	{
		if (tesselationFactor >= 0.1)
		{
			tesselationFactor -= 0.1;
		}
	}
	if (m_Input->isKeyDown('X'))
	{
		if (tesselationFactor < 64)
		{
			tesselationFactor += 0.1;
		}
		
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

void BaseApplication::setCamera(DisplayState newState)
{
	switch (newState)
	{
	case MOUNTAINS:
		m_Camera->SetPosition(5, 5, -15);
		m_Camera->SetRotation(40, 0, 0);
		m_Camera->Update();
		break;
	case LIGHTING:
		break;
	default:
		break;
	}
}