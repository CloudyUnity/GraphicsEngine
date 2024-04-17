cbuffer TessellationBuffer : register (b0)
{
    float tessellationAmount;
    float lodFactor;
    float2 tessPad;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
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

ConstantOutputType PatchConstantFunction(InputPatch<HullInputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    float lod = lodFactor / length(inputPatch[0].position.xyz - cameraPosition.xyz);

    output.edges[0] = tessellationAmount * lod;
    output.edges[1] = tessellationAmount * lod;
    output.edges[2] = tessellationAmount * lod;

    output.inside = tessellationAmount * lod;

    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]

DomainInputType HS_MAIN(InputPatch<HullInputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    DomainInputType output;

    output.position = patch[pointId].position;
    output.tex = patch[pointId].tex;

    return output;
}