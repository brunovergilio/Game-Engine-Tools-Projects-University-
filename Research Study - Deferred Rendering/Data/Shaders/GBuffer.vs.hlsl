cbuffer CBObject : register(b0)
{
	float4x4 gWVP;
	float4x4 gWorld;
	float4x4 gInvWorld;
};

struct VertexIn
{
    float3 position  : POSITION;
    float2 tex       : TEXCOORD;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
};

struct VertexOut
{
    float4 posH      : SV_POSITION;
    float2 tex       : TEXCOORD;
    float3 posW      : POSITION;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
};

struct VertexOutPos
{
    float4 posH      : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.posH = mul(gWVP, float4(vin.position, 1.0f));

    vout.tex = vin.tex;

    vout.posW   = mul(gWorld, float4(vin.position, 1.0f)).xyz;
    vout.normal = mul((float3x3)gInvWorld, vin.normal);

    vout.tangent = mul((float3x3)gWorld, vin.tangent);
    
    return vout;
}

VertexOutPos VS_PosOnly(VertexIn vin)
{
    VertexOutPos vout;
    vout.posH = mul(gWVP, float4(vin.position, 1.0f));

    return vout;
}