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

cbuffer cbSkin : register(b1)
{
	float4x4 gFinalMatrices[100];
};

struct VertexIn
{
    float3 position   : POSITION;
    float2 tex        : TEXCOORD0;
    float3 normal     : NORMAL;
    float3 tangent    : TANGENT;
    float3 bitangent  : BITANGENT;
    uint4 boneindices : BONEINDICES;
    float4 weights    : WEIGHTS;
};

struct VertexOut
{
    float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float2 tex     : TEXTURE;
    float3 normal  : NORMAL;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    weights[0] = vin.weights.x;
    weights[1] = vin.weights.y;
    weights[2] = vin.weights.z;
    weights[3] = vin.weights.w;//1.0f - weights[0] - weights[1] - weights[2];

    float3 position = float3(0.0f, 0.0f, 0.0f);
    float3 normal   = float3(0.0f, 0.0f, 0.0f);
    float3 tangent  = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; ++i)
    {
	position += weights[i] * mul(gFinalMatrices[vin.boneindices[i]], float4(vin.position, 1.0f)).xyz;
	normal   += weights[i] * mul((float3x3)gFinalMatrices[vin.boneindices[i]], vin.normal);
	tangent  += weights[i] * mul((float3x3)gFinalMatrices[vin.boneindices[i]], vin.tangent);
    }

    // Transform to homogeneous clip space.
    vout.posH = mul(gWVP, float4(position, 1.0f));

    vout.posW    = mul(gWorld, float4(position, 1.0f)).xyz;
    vout.normal  = mul((float3x3)gInvWorld, normal);
    
    return vout;
}