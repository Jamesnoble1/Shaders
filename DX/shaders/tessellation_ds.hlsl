// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);


cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
	float2 tex : TEXCOORD0;
};

[domain("quad")]

OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 uvPos;
	float height;
	
	OutputType output;

	//calculates vertex position
	float3 v1 = lerp(patch[0].position, patch[1].position, 1 - uvwCoord.y);
	float3 v2 = lerp(patch[2].position, patch[3].position, 1 - uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	//calculates the UVs for the created vertex
	float3 uv1 = lerp(patch[0].position, patch[1].position,  uvwCoord.y);
	float3 uv2 = lerp(patch[2].position, patch[3].position,  uvwCoord.y);
	uvPos =  lerp(uv1, uv2, uvwCoord.x);

	//samples height from displacement map 
	height = 1 - texture0.SampleLevel(Sampler0, uvPos, 0);

	vertexPosition.z = vertexPosition.z + height;

	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Send the input color into the pixel shader.
	output.colour = height;
	output.tex = uvPos;
	
    return output;
}

