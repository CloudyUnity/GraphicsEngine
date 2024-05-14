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

cbuffer MatrixBuffer : register(b2)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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

    float4 pos;
    pos.xyz = (inputPatch[0].position.xyz + inputPatch[1].position.xyz + inputPatch[2].position.xyz + inputPatch[3].position.xyz) / 4.0f;
    pos.w = 1;
    pos = mul(pos, worldMatrix);
    pos.xyz /= pos.w;

    float4 poss[4];
    for (int i = 0; i < 4; i++){
        poss[i].xyz = inputPatch[i].position.xyz;
        poss[i].w = 1;
        poss[i] = mul(poss[i], worldMatrix);
        poss[i].xyz /= pos.w;
    }

    float lod = lodFactor / length(pos - cameraPosition.xyz);

    output.edges[0] = tessellationAmount * lodFactor / length(poss[0] - cameraPosition.xyz);
    output.edges[1] = tessellationAmount * lodFactor / length(poss[1] - cameraPosition.xyz);
    output.edges[2] = tessellationAmount * lodFactor / length(poss[2] - cameraPosition.xyz);
    output.edges[3] = tessellationAmount * lodFactor / length(poss[3] - cameraPosition.xyz);

    output.inside[0] = tessellationAmount * lod;
    output.inside[1] = tessellationAmount * lod;

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