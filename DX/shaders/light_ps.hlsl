// Light pixel shader
// Calculate ambient and diffuse lighting for a single light also colours pixel based on y position
//based on lighting shader created for Shaders module by Myself

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(cb0)
{
    float4 diffuseColour[3];
	float4 ambientColour[3];
	float3 lightLocationOne;
	float paddingOne;
	float3 lightLocationTwo;
	float paddingTwo;
	float3 lightLocationThree;
	float paddingThree;
};
 
//buffer to hold values for colour passed in. Allows multple pallets and changing cutoffs
//num values only uses .x rest is for padding
cbuffer colouringBuffer: register(cb1)
{
	float4 levelColours[16];
	float4 numValues;
	float4 levelCutoffs[16];
	
};

struct InputType
{
    float4 position : SV_POSITION;
	float4 originalPos : TEXCOORD3;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

//calculate colour of pixel based on y value
float4 calculateColour(float yValue)
{
	float4 colour;

	//first
	if (yValue >= levelCutoffs[0].x)
	{
		colour = levelColours[0];
	}

	//the ones inbetween
	for (int i = 1; i < (int(numValues.x) ); i++)
	{
		if (yValue >= levelCutoffs[i].x && yValue <= levelCutoffs[i-1].x)
		{
			//lerp between two colours
			colour = lerp(levelColours[i], levelColours[i - 1], (yValue - levelCutoffs[i]) / (levelCutoffs[i - 1] - levelCutoffs[i]));
		}
	}

	//last
	if (yValue <= levelCutoffs[int(numValues.x) - 1].x)
	{
		colour = levelColours[int(numValues.x) - 1];
	}
	
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float3 lightVector;
    float lightIntensity;
    float4 colour;
	float4 diffuseValue;
	float attenuationValue;
	float distance;

	//constants for attenuation. May have these controlable by user
	float maxDistance = 100.0f;
	float constantFactor = 1.0f;
	float linearFactor = 0.125f;
	float quadratciFactor = 0.0f;

	textureColour = calculateColour(input.originalPos.y);
	
	for (int currentLight = 0; currentLight < 3; currentLight++)
	{
		colour += ambientColour[currentLight];

		switch (currentLight)
		{
			case 0:
				// Invert the light direction for calculations.
				lightVector = input.position3D - lightLocationOne;
				break;
		
		
			case 1:
				// Invert the light direction for calculations.
				lightVector = input.position3D - lightLocationTwo;
				break;
		
		
			case 2:
				// Invert the light direction for calculations.
				lightVector = input.position3D - lightLocationThree;
				break;

			default:
				break;
		}

		if (currentLight == 0)
		{
			distance = 20;
		}
		else
		{
			distance = length(lightVector);
		}
			
		if (distance < maxDistance)
		{
			normalize(lightVector);

			// Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(input.normal, -lightVector));

			if (lightIntensity > 0)
			{
				// Determine the  diffuse color based on the diffuse color combined with the light intensity.
				diffuseValue = diffuseColour[currentLight] * lightIntensity;
				attenuationValue = 1/ (constantFactor + (linearFactor * distance) + (quadratciFactor * distance * distance));
				colour += saturate(diffuseValue * attenuationValue);
			}
		}
		
	}
	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	colour = colour * textureColour;
	colour = saturate(colour);
	return colour;
}

