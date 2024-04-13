#define NUM_LIGHTS 4

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct HullInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

HullInputType VS_MAIN(VertexInputType input)
{
    HullInputType output;

    output.position = input.position;  
    output.tex = input.tex;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.binormal = input.binormal;

    return output;
}