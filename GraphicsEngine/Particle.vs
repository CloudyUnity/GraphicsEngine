cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer PaddingBuffer : register(b1)
{
    float4 p;
}

cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition;
    float padding;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;        
    float4 color : COLOR;
};

PixelInputType VS_MAIN(VertexInputType input)
{
    PixelInputType output;
    
    input.position.w = 1.0f;

    // float angle = atan2(input.position.x - cameraPosition.x, input.position.z - cameraPosition.z);

    // matrix rotateMatrix = float4x4(
    // cos(angle), 0, -sin(angle), 0,
    // 0, 1, 0, 0, 
    // sin(angle), 0, cos(angle), 0,
    // 0, 0, 0, 1
    // );

    // output.position = mul(mul(input.position, rotateMatrix), worldMatrix);

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;   

    output.color = input.color;

    return output;
}