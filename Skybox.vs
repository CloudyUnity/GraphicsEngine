cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD0;        
};

PixelInputType VS_MAIN(VertexInputType input)
{
    PixelInputType output;
    
    input.position.w = 1.0f;

    output.position = mul(mul(mul(input.position, worldMatrix), viewMatrix), projectionMatrix);

    // By doing this depth will always be 1.0 (max)
    output.position.z = output.position.w;
    
    output.tex = input.position.xyz;   

    return output;
}