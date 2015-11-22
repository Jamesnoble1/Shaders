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

	tempColour = shaderTexture.Sample(SampleType, input.tex);

	tempColour.g = 0;

	return tempColour;
}