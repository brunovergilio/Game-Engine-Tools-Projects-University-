struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular; // w = SpecPower
	float4 reflect;
	float4 color;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWVP;
	float4x4 gWorld;
	float4x4 gInvWorld;
	Material gMaterial;
};

struct VertexIn
{
    float3 position  : POSITION;
    float2 tex       : TEXCOORD0;
    float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VertexOut
{
    float4 posH      : SV_POSITION;
    float2 tex       : TEXCOORD0;
    float3 posW      : POSITION;
    float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
	float3 bitangent : BITANGENT;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
	
    // Transform to homogeneous clip space.
    vout.posH = mul(gWVP, float4(vin.position, 1.0f));

    // Just pass vertex color into the pixel shader.
    vout.tex = vin.tex;

    vout.posW   = mul(gWorld, float4(vin.position, 1.0f)).xyz;
    vout.normal = mul((float3x3)gInvWorld, vin.normal);

	vout.tangent = mul((float3x3)gWorld, vin.tangent);
	vout.bitangent = mul((float3x3)gWorld, vin.bitangent);
    
    return vout;
}