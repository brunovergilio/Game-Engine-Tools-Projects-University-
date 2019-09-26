cbuffer CBObject : register(b0)
{
	float4x4 gViewProj;
};

struct VertexIn
{
    float3 position   : POSITION;
    float2 tex        : TEXCOORD;
    float3 normal     : NORMAL;
    float3 tangent    : TANGENT;
    float4x4 world    : WORLD;
    float4x4 invWorld : INVWORLD;
};

struct VertexOut
{
    float4 posH      : SV_POSITION;
    float2 tex       : TEXCOORD;
    float3 posW      : POSITION;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout;

    vout.tex = vin.tex;

    float4 posH = mul(vin.world, float4(vin.position, 1.0f));

    vout.posH = mul(gViewProj, posH);
    vout.posW   = posH.xyz;
    vout.normal = mul((float3x3)vin.invWorld, vin.normal);
    vout.tangent = mul((float3x3)vin.world, vin.tangent);

    return vout;
}