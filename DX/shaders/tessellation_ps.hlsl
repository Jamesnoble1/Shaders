// Tessellation pixel shader
// Output colour passed to stage.

Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
	float4 finalColour;

	textureColour = texture0.Sample(Sampler0, input.tex);

	finalColour.w = textureColour.w ;
	finalColour.x = textureColour.x ;
	finalColour.y = textureColour.y ;
	finalColour.z = textureColour.z ;
	
	
	return finalColour;
}