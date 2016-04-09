
cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[3];
	matrix lightProjectionMatrix[3];
};

cbuffer LightBuffer2 : register(cb1)
{
    float3 lightPositionOne;
	float paddingOne;
	float3 lightPositionTwo;
	float paddingTwo;
	float3 lightPositionThree;
	float paddingThree;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
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


OutputType main(InputType input)
{
    OutputType output;
	float4 worldPosition;
        
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// Normalize the normal vector.
	output.normal = normalize(output.normal);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(input.position, worldMatrix);

	//switch to calculate all lighting info
	for (int currentLight = 0; currentLight < 3; currentLight++)
	{
		switch (currentLight)
		{
		case 0:
			// Calculate the position of the vertice as viewed by the light source.
			output.lightViewPositionOne = mul(input.position, worldMatrix);
			output.lightViewPositionOne = mul(output.lightViewPositionOne, lightViewMatrix[0]);
			output.lightViewPositionOne = mul(output.lightViewPositionOne, lightProjectionMatrix[0]);

			// Determine the light position based on the position of the light and the position of the vertex in the world.
			output.lightPosOne = lightPositionOne.xyz - worldPosition.xyz;

			// Normalize the light position vector.
			output.lightPosOne = normalize(output.lightPosOne);
			break;
		case 1:
			// Calculate the position of the vertice as viewed by the light source.
			output.lightViewPositionTwo = mul(input.position, worldMatrix);
			output.lightViewPositionTwo = mul(output.lightViewPositionTwo, lightViewMatrix[1]);
			output.lightViewPositionTwo = mul(output.lightViewPositionTwo, lightProjectionMatrix[1]);

			// Determine the light position based on the position of the light and the position of the vertex in the world.
			output.lightPosTwo = lightPositionTwo.xyz - worldPosition.xyz;

			// Normalize the light position vector.
			output.lightPosTwo = normalize(output.lightPosTwo);
			break;
		case 2:
			// Calculate the position of the vertice as viewed by the light source.
			output.lightViewPositionThree = mul(input.position, worldMatrix);
			output.lightViewPositionThree = mul(output.lightViewPositionThree, lightViewMatrix[2]);
			output.lightViewPositionThree = mul(output.lightViewPositionThree, lightProjectionMatrix[2]);

			// Determine the light position based on the position of the light and the position of the vertex in the world.
			output.lightPosThree = lightPositionThree.xyz - worldPosition.xyz;

			// Normalize the light position vector.
			output.lightPosThree = normalize(output.lightPosThree);
			break;
		}
		
	}

	return output;
}