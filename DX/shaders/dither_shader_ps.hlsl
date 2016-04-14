// ordered dithering shader that mimics the effect of dithering used when full colour depth doesn't exist.
//only works when screen size 800 * 600. May increase to work with any size
Texture2D shaderTexture : register(t0); 
SamplerState SampleType : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

//checks if colour passes the threshold
float checkColour(int xPos, int yPos, float colour)
{
	//dither array for threshold values
	int dither[8][8] = {
		{ 0, 32, 8, 40, 2, 34, 10, 42 },
		{ 48, 16, 56, 24, 50, 18, 58, 26 },
		{ 12, 44, 4, 36, 14, 46, 6, 38 },
		{ 60, 28, 52, 20, 62, 30, 54, 22 },
		{ 3, 35, 11, 43, 1, 33, 9, 41 },
		{ 51, 19, 59, 27, 49, 17, 57, 25 },
		{ 15, 47, 7, 39, 13, 45, 5, 37 },
		{ 63, 31, 55, 23, 61, 29, 53, 21 } };

	//find threshold and turn into a value between 0 and 1
	float threshold = (dither[xPos][yPos] + 1) / 64.0;
	
	if (colour > threshold)
	{
		return 1.0;
	}
	else
	{
		return 0.0;
	}
}

float4 main(InputType input) : SV_TARGET
{


	float4 sampleColour = shaderTexture.Sample(SampleType, input.tex);

	float4 finalColour;

	//u * screen width and v * screen height to turn from UV to pixel
	finalColour.r = checkColour(int((input.tex.x * 800) % 8), int((input.tex.y * 600) % 8), sampleColour.r);
	finalColour.g = checkColour(int((input.tex.x * 800) % 8), int((input.tex.y * 600) % 8), sampleColour.g);
	finalColour.b = checkColour(int((input.tex.x * 800) % 8), int((input.tex.y * 600) % 8), sampleColour.b);
	finalColour.a = 1.0f;

	return finalColour;
}