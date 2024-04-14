cbuffer TessellationBuffer : register (b0)
{
    float tessellationAmount;
    float3 padding;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct HullInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct DomainInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

ConstantOutputType PatchConstantFunction(InputPatch<HullInputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    output.edges[0] = tessellationAmount;
    output.edges[1] = tessellationAmount;
    output.edges[2] = tessellationAmount;
    output.edges[3] = tessellationAmount;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside[0] = tessellationAmount;
    output.inside[1] = tessellationAmount;

    return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]

DomainInputType HS_MAIN(InputPatch<HullInputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DomainInputType output;

    output.position = patch[pointId].position;
    output.tex = patch[pointId].tex;

    return output;
}