#define NUM_LIGHTS 4

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer LightPositionBuffer
{
    float4 lightPosition[NUM_LIGHTS];
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 lightPos[NUM_LIGHTS] : TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;
    
    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    output.normal = normalize(output.normal);

    worldPosition = mul(input.position, worldMatrix);

    for(int i=0; i<NUM_LIGHTS; i++)
    {
        // Determine the light positions based on the position of the lights and the position of the vertex in the world.
        output.lightPos[i] = normalize(lightPosition[i].xyz - worldPosition.xyz);
    }

    return output;
}