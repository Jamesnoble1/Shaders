// Light pixel shader
// Calculate ambient and diffuse lighting for a single light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(cb0)
{
    float4 diffuseColour[4];
	float4 ambient[4];
    float3 lightDirection[4];
	float3 lightPosition[4];
    float padding[4];
	float padding2[4];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float3 lightDir;
    float lightIntensity;
    float4 colour;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColour =  shaderTexture.Sample(SampleType, input.tex);
	
	// Invert the light direction for calculations.
   // lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    //lightIntensity = saturate(dot(input.normal, lightDir));
	colour = ambient[0];
	for (int i = 0; i < 4; i++)
	{
		// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
		//colour = saturate(diffuseColour * lightIntensity);
		
		// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
		//colour = colour * textureColour;
		lightDir = normalize(input.position3D - lightPosition[i]);

		// Calculate the amount of light on this pixel.
		lightIntensity = saturate(dot(input.normal, -lightDir));

		if (lightIntensity > 0.0f)
		{
			// Determine the final diffuse color based on the diffuse color and the amount 	of light intensity.
			colour += (diffuseColour[i] * lightIntensity);

			// Saturate the ambient and diffuse color.
			colour = saturate(colour);
		}

		// Multiply the texture pixel and the final diffuse colour
		colour = colour * textureColour;
	}
	return colour;
}

