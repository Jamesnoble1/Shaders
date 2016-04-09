//basic chromakey pixel shader just replaces green with texture. Plan to expand to user controlable colour
Texture2D shaderTexture : register(t0);
Texture2D replaceTexture : register(t1);
SamplerState SampleType : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	float4 tempColour;
	float4 compareColour;

	
	tempColour = shaderTexture.Sample(SampleType, input.tex);
	
	//filters all greens above 0.2. Change to need a higher green value to filter. Replaces it with replaceTexture
	if (tempColour.g > 0.2)
	{
		tempColour = replaceTexture.Sample(SampleType, input.tex);
	}

	return tempColour;
}