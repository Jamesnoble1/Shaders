//basic grey scale pixel shader
Texture2D shaderTexture : register(t0);
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
	float4 colour;
	float greyColour;
	
	tempColour = shaderTexture.Sample(SampleType, input.tex);

	greyColour = (tempColour.r + tempColour.g + tempColour.b) / 3;
	colour.a = tempColour.a;
	colour.r = greyColour;
	colour.g = greyColour;
	colour.b = greyColour;

	return colour;
}