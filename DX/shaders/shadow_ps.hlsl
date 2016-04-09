
Texture2D shaderTexture : register(t0);
Texture2D depthMapTextureOne : register(t1);
Texture2D depthMapTextureTwo : register(t2);
Texture2D depthMapTextureThree : register(t3);

SamplerState SampleTypeWrap  : register(s0);
SamplerState SampleTypeClamp : register(s1);

cbuffer LightBuffer : register(cb0)
{
	float4 ambientColor[3];
	float4 diffuseColor[3];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 lightViewPositionOne : TEXCOORD1;
	float4 lightViewPositionTwo : TEXCOORD2;
	float4 lightViewPositionThree : TEXCOORD3;
	float3 lightPosOne : TEXCOORD4;
	float3 lightPosTwo : TEXCOORD5;
	float3 lightPosThree : TEXCOORD6;
};


float4 main(InputType input) : SV_TARGET
{
	float bias;
    float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
    float lightIntensity;
	float4 textureColor;
	
	// Set the bias value for fixing the floating point precision issues.
	bias = 0.0001f;
	color = ambientColor[0];
	for (int lightNumber = 0; lightNumber < 3; lightNumber++)
	{

		// Set the default output color to the ambient light value for all pixels.
		
		// Calculate the projected texture coordinates.
		switch (lightNumber)
		{
		case 0:
			projectTexCoord.x = input.lightViewPositionOne.x / input.lightViewPositionOne.w / 2.0f + 0.5f;
			projectTexCoord.y = -input.lightViewPositionOne.y / input.lightViewPositionOne.w / 2.0f + 0.5f;
			break;
		case 1:
			projectTexCoord.x = input.lightViewPositionTwo.x / input.lightViewPositionTwo.w / 2.0f + 0.5f;
			projectTexCoord.y = -input.lightViewPositionTwo.y / input.lightViewPositionTwo.w / 2.0f + 0.5f;
			break;
		case 2:
			projectTexCoord.x = input.lightViewPositionThree.x / input.lightViewPositionThree.w / 2.0f + 0.5f;
			projectTexCoord.y = -input.lightViewPositionThree.y / input.lightViewPositionThree.w / 2.0f + 0.5f;
			break;
		}
		
		// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
		if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
		{
						
			// Calculate the depth of the light. &  Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
			switch (lightNumber)
			{
			case 0:
				depthValue = depthMapTextureOne.Sample(SampleTypeClamp, projectTexCoord).r;
				lightDepthValue = input.lightViewPositionOne.z / input.lightViewPositionOne.w;
				break;
			case 1:
				depthValue = depthMapTextureTwo.Sample(SampleTypeClamp, projectTexCoord).r;
				lightDepthValue = input.lightViewPositionTwo.z / input.lightViewPositionTwo.w;
				break;
			case 2:
				depthValue = depthMapTextureThree.Sample(SampleTypeClamp, projectTexCoord).r;
				lightDepthValue = input.lightViewPositionThree.z / input.lightViewPositionThree.w;
				break;
			}
			
			// Subtract the bias from the lightDepthValue.
			lightDepthValue = lightDepthValue - bias;

			// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
			// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
			if (lightDepthValue < depthValue)
			{
				switch (lightNumber)
				{
				case 1:
					// Calculate the amount of light on this pixel.
					lightIntensity = saturate(dot(input.normal, input.lightPosOne));
					break;
				case 2:
					// Calculate the amount of light on this pixel.
					lightIntensity = saturate(dot(input.normal, input.lightPosTwo));
					break;
				case 3:
					// Calculate the amount of light on this pixel.
					lightIntensity = saturate(dot(input.normal, input.lightPosThree));
					break;
				}
				

				if (lightIntensity > 0.0f)
				{
					// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
					color += (diffuseColor[lightNumber] * lightIntensity);

					// Saturate the final light color.
					color = saturate(color);
				}
			}
		}
	}
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// Combine the light and texture color.
	color = saturate(color * textureColor);

    return color;
}