// Application.h
#ifndef PROJECT_H
#define PROJECT_H

#define PI 3.14159
// Includes

#include "baseapplication.h"


//includes for tesselation
#include "TessellationMesh.h"
#include "TessellationShader.h"

//includes for render to target/postprocessing
#include "OrthoMesh.h"
#include "TextureShader.h"
#include "RenderTexture.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "ColourFilter.h"

//includes for lighting
#include "PlaneMesh.h"
#include "SphereMesh.h"
#include "CubeMesh.h"

class Project : public BaseApplication
{
public:

	Project(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
	~Project();

	bool Frame();

private:
	bool Render();
	
	void drawOptions(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix);
	
	
	//scenes
	void renderMountians(const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix); //renders displacement map mountain scene
	void renderLightingTest(const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix); //renders test for shadows and lighting
	
	//post proccessing
	void downScale(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix); //renders to a texture half the size of the window
	void horizontalBlurFunction(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix); //blurs horizontaly
	void vertBlurFunction(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix); //blurs verticaly
	void upScale(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix); //renders to texture size of screen
	void drawBlur(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix); //Draws the blurred texture on an ortho plane
	void applyFilter(const XMMATRIX & worldMatrix, const XMMATRIX & baseVeiwMatrix, const XMMATRIX & orthoMatrix); //applies colour filter controlled by filter state
	void RenderToTexture(const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix); //renders a scene to a texture

private:
	//render to textures
	RenderTexture* initialScene;
	RenderTexture* downSample;
	RenderTexture* horizBlur;
	RenderTexture* vertBlur;
	RenderTexture* upSample;

	
	//meshes
	OrthoMesh* optionsMenu; //options menu
	OrthoMesh* postProcess; //used for post processing

	//used fo mountain scene
	TessellationMesh* terrainMesh;

	//used for lighting test
	//different sphere meshes as will have different textures. Otherwise would have translated same sphere
	SphereMesh* firstSphere;
	SphereMesh* secondSphere;
	SphereMesh* thirdSphere;
	//Cube for podium
	CubeMesh* podiumCube;
	//plane for ground
	PlaneMesh* groundMesh;

	//shader classes
	TessellationShader* testAlationShader;
	TextureShader* textureShader;
	ColourFilterShader* greyScaleFilter;
	ColourFilterShader* redFilter;
	ColourFilterShader* greenFilter;
	ColourFilterShader* blueFilter;
	HorizontalBlurShader* horizontalBlurShader;
	VerticalBlurShader* vertBlurShader;
	
	int screenWidth;
	int screenHeight;

	
};

#endif