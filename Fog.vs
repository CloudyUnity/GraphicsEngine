#define NUM_LIGHTS 4

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer LightPositionBuffer : register(b1)
{
    float4 lightPosition[NUM_LIGHTS];
};

cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition;
    float padding;
};

cbuffer FogBuffer : register(b3)
{
    float fogStart;
    float fogEnd;
};

cbuffer ClipPlaneBuffer : register(b4)
{
    float4 clipPlane;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION0;
    float4 worldPos : POSITION1;

    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;    
    float3 viewDirection : TEXCOORD1;
    float fogFactor : FOG;
    float clip : SV_ClipDistance0;

    float3 lightPos[NUM_LIGHTS] : TEXCOORD2;
};

PixelInputType VS_MAIN(VertexInputType input)
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

    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);

    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    output.binormal = normalize(output.binormal);

    worldPosition = mul(input.position, worldMatrix);
    output.worldPos = worldPosition;

    for(int i=0; i<NUM_LIGHTS; i++)
    {
        // Determine the light positions based on the position of the lights and the position of the vertex in the world.
        output.lightPos[i] = normalize(lightPosition[i].xyz - worldPosition.xyz);
    }

    output.viewDirection = normalize(cameraPosition.xyz - worldPosition.xyz);

    float4 cameraPos = mul(input.position, worldMatrix);
    cameraPos = mul(cameraPos, viewMatrix);

    output.fogFactor = saturate((fogEnd - cameraPos.z) / (fogEnd - fogStart));
    // Exponential Fog = 1.0 / 2.71828 power (ViewpointDistance * FogDensity)

    output.clip = dot(mul(input.position, worldMatrix), clipPlane);

    return output;
}