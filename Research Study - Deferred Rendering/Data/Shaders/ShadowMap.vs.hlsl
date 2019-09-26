cbuffer CBObject : register(b0)
{
	float4x4 gWVP;
};

struct VertexIn
{
    float3 position  : POSITION;
    float2 tex       : TEXCOORD;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
};

struct VertexOutPos
{
    float4 posH      : SV_POSITION;
};

VertexOutPos VS_PosOnly(VertexIn vin)
{
    VertexOutPos vout;
    vout.posH = mul(gWVP, float4(vin.position, 1.0f));

    return vout;
}

float4 VS_WorldPosOnly(VertexIn vin) : SV_POSITION
{
    return float4(vin.position, 1.0f);
}