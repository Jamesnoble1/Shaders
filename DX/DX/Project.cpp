#include "Project.h"

Project::Project(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in) : BaseApplication(hinstance, hwnd, screenWidth, screenHeight, in)
{
	// Create Mesh object
	//creates terain mesh
	terrainMesh = new TessellationMesh(m_Direct3D->GetDevice(), L"../res/mountainTex.png", L"../res/mountainDispMap.png");

	//create ortho mesh for options menu
	optionsMenu = new OrthoMesh(m_Direct3D->GetDevice(), 200, 200, L"../res/options.png");

	//full screen orth mesh with no texture for post processing
	postProcess = new OrthoMesh(m_Direct3D->GetDevice(), screenWidth, screenHeight);

	//lighting meshes
	//different sphere meshes as will have different textures. Otherwise would have translated same sphere
	firstSphere = new SphereMesh(m_Direct3D->GetDevice(), L"../res/colour test.png", 50);
	secondSphere = new SphereMesh(m_Direct3D->GetDevice(), L"../res/colour test.png");
	thirdSphere = new SphereMesh(m_Direct3D->GetDevice(), L"../res/colour test.png");
	//Cube for podium
	podiumCube = new CubeMesh(m_Direct3D->GetDevice(), L"../res/brick1.dds");
	//plane for ground
	groundMesh = new PlaneMesh(m_Direct3D->GetDevice(), L"../res/bunny.png");

	//create shader objects
	testAlationShader = new TessellationShader(m_Direct3D->GetDevice(), hwnd);
	textureShader = new TextureShader(m_Direct3D->GetDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(m_Direct3D->GetDevice(), hwnd);
	vertBlurShader = new VerticalBlurShader(m_Direct3D->GetDevice(), hwnd);
	greyScaleFilter = new ColourFilterShader(m_Direct3D->GetDevice(), hwnd, L"../shaders/grey_scale_ps.hlsl");
	
	//filters set the appropriate channel to 0 
	redFilter = new ColourFilterShader(m_Direct3D->GetDevice(), hwnd, L"../shaders/red_filter_ps.hlsl");
	greenFilter = new ColourFilterShader(m_Direct3D->GetDevice(), hwnd, L"../shaders/green_filter_ps.hlsl");
	blueFilter = new ColourFilterShader(m_Direct3D->GetDevice(), hwnd, L"../shaders/blue_filter_ps.hlsl");

	//render to textures
	initialScene = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downSample = new RenderTexture(m_Direct3D->GetDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	horizBlur = new RenderTexture(m_Direct3D->GetDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	vertBlur = new RenderTexture(m_Direct3D->GetDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	upSample = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//stores wcreen width and height
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	
}


Project::~Project()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (terrainMesh)
	{
		delete terrainMesh;
	}

	if (testAlationShader)
	{
		delete testAlationShader;
		
	}
}


bool Project::Frame()
{
	bool result;

	result = BaseApplication::Frame();
	if (!result)
	{
		return false;
	}

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
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseVeiwMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	

	if (blurUpdated && isHighBlur)
	{
		horizontalBlurShader->changeShaderBlur(true);
		vertBlurShader->changeShaderBlur(true);
		blurUpdated = false;
	}
	else
	{
		horizontalBlurShader->changeShaderBlur(false);
		vertBlurShader->changeShaderBlur(false);
		blurUpdated = false;
	}

	switch (currentState)
	{
	case START:
	{
				  drawOptions(worldMatrix, baseVeiwMatrix, orthoMatrix);
		break;	
	}
			
		case MOUNTAINS:
		{
			if (shouldBlur)
			{
			
				//renders scene to texture
				RenderToTexture(worldMatrix, viewMatrix, projectionMatrix);

				//down samples
				downScale(worldMatrix, baseVeiwMatrix, orthoMatrix);

				//Blurs horizontally
				horizontalBlurFunction(worldMatrix, baseVeiwMatrix, orthoMatrix);

				//blurs verticaly
				vertBlurFunction(worldMatrix, baseVeiwMatrix, orthoMatrix);

				//upscales to screen size
				upScale(worldMatrix, baseVeiwMatrix, orthoMatrix);

				//draws finished image
				drawBlur(worldMatrix, baseVeiwMatrix, orthoMatrix);
			}
			else if (currentFilter != NONE)
			{
				//renders scene to texture
				RenderToTexture(worldMatrix, viewMatrix, projectionMatrix);
				applyFilter(worldMatrix, baseVeiwMatrix, orthoMatrix);
				
			}
			else
			{
				renderMountians(worldMatrix, viewMatrix, projectionMatrix);
			}
		
			break;
		}

		case LIGHTING:
		{
			break;
		}
	}

	if (showOptions)
	{
		drawOptions(worldMatrix, baseVeiwMatrix, orthoMatrix);
	}
	m_Direct3D->EndScene();
	return true;
}


void Project::drawOptions(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix)
{
	m_Direct3D->TurnZBufferOff();
	optionsMenu->SendData(m_Direct3D->GetDeviceContext());

	textureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, optionsMenu->GetTexture());

	textureShader->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());
	m_Direct3D->TurnZBufferOn();
}

void Project::RenderToTexture(const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix)
{
	//Set render target to texture
	initialScene->SetRenderTarget(m_Direct3D->GetDeviceContext());

	//Clear the render to texture
	initialScene->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//make sure cameraposition is updated
	m_Camera->Update();

	renderMountians(worldMatrix, viewMatrix, projectionMatrix);

	m_Direct3D->SetBackBufferRenderTarget();
}

void Project::renderMountians(const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix)
{
	XMMATRIX startWorld = worldMatrix;
	XMMATRIX scaledMatrix = worldMatrix;
	// Send geometry data (from mesh)
	terrainMesh->SendData(m_Direct3D->GetDeviceContext());

	//scaling tesselated object
	scaledMatrix = worldMatrix * XMMatrixScaling(10, 10, 10) * XMMatrixRotationRollPitchYaw(PI / 2, 0, 0);

	testAlationShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), scaledMatrix, viewMatrix, projectionMatrix, terrainMesh->GetTexture(), terrainMesh->getDisplacement(), tesselationFactor);
	// Present the rendered scene to the screen.
	testAlationShader->Render(m_Direct3D->GetDeviceContext(), terrainMesh->GetIndexCount());
	scaledMatrix = startWorld;
}

void renderLightingTest(const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix)
{

}


void Project::downScale(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix)
{
	//Set render target to texture
	downSample->SetRenderTarget(m_Direct3D->GetDeviceContext());

	//Clear the render to texture
	downSample->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//make sure cameraposition is updated
	m_Camera->Update();

	//draws an ortho mesh with initial scenes stored texture
	postProcess->SendData(m_Direct3D->GetDeviceContext());

	textureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, initialScene->GetShaderResourceView());

	textureShader->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());

	m_Direct3D->SetBackBufferRenderTarget();
}

void Project::horizontalBlurFunction(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix)
{
	//Set render target to texture
	horizBlur->SetRenderTarget(m_Direct3D->GetDeviceContext());

	//Clear the render to texture
	horizBlur->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//make sure cameraposition is updated
	m_Camera->Update();

	//draws an ortho mesh with downScales scenes stored texture
	postProcess->SendData(m_Direct3D->GetDeviceContext());

	horizontalBlurShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, downSample->GetShaderResourceView(), screenWidth/2);

	horizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());

	m_Direct3D->SetBackBufferRenderTarget();
}

void Project::vertBlurFunction(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix)
{
	//Set render target to texture
	vertBlur->SetRenderTarget(m_Direct3D->GetDeviceContext());

	//Clear the render to texture
	vertBlur->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//make sure cameraposition is updated
	m_Camera->Update();

	//draws an ortho mesh with horizBlur  stored texture
	postProcess->SendData(m_Direct3D->GetDeviceContext());

	vertBlurShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, horizBlur->GetShaderResourceView(), screenHeight/2);

	vertBlurShader->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());

	m_Direct3D->SetBackBufferRenderTarget();
}

void Project::upScale(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix)
{
	//Set render target to texture
	upSample->SetRenderTarget(m_Direct3D->GetDeviceContext());

	//Clear the render to texture
	upSample->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//draws an ortho mesh with vert blur scenes stored texture
	postProcess->SendData(m_Direct3D->GetDeviceContext());

	textureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, vertBlur->GetShaderResourceView());

	textureShader->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());

	m_Direct3D->SetBackBufferRenderTarget();
}

void Project::drawBlur(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix)
{
	
	if (currentFilter != NONE)
	{
		//Set render target to texture
		initialScene->SetRenderTarget(m_Direct3D->GetDeviceContext());

		//Clear the render to texture
		initialScene->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

		//draws an ortho mesh with vert blur scenes stored texture
		postProcess->SendData(m_Direct3D->GetDeviceContext());

		textureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, vertBlur->GetShaderResourceView());

		textureShader->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());

		m_Direct3D->SetBackBufferRenderTarget();

		applyFilter(worldMatrix, baseVeiwMatrix, orthoMatrix);
	}
	else
	{
		//draws an ortho mesh with vert blur scenes stored texture
		postProcess->SendData(m_Direct3D->GetDeviceContext());

		textureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, vertBlur->GetShaderResourceView());

		textureShader->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());
	}
	

}

void Project::applyFilter(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix)
{
	postProcess->SendData(m_Direct3D->GetDeviceContext());
	switch (currentFilter)
	{
	case NONE:
		break;
	case GREYSCALE:
		greyScaleFilter->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, initialScene->GetShaderResourceView());

		greyScaleFilter->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());
		break;
	case NO_RED:
		redFilter->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, initialScene->GetShaderResourceView());
		redFilter->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());
		break;
	case NO_GREEN:
		greenFilter->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, initialScene->GetShaderResourceView());
		greenFilter->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());
		break;
	case NO_BLUE:
		blueFilter->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseVeiwMatrix, orthoMatrix, initialScene->GetShaderResourceView());
		blueFilter->Render(m_Direct3D->GetDeviceContext(), optionsMenu->GetIndexCount());
		break;
	}
	

}