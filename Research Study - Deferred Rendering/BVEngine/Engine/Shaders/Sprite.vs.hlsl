cbuffer CBVertexData : register(b0)
{
    float4x4 gOrtho;
    float2 gTextureInfo;
};

struct VertexIn
{
    float3 position : POSITION;
    float2 tex      : TEXCOORD0;
};

struct VertexOut
{
    float4 posH   : SV_POSITION;
    float2 tex    : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.tex.x = vin.tex.x / gTextureInfo.x;
    vout.tex.y = vin.tex.y / gTextureInfo.y;

    vout.posH = mul(gOrtho, float4(vin.position, 1.0f));
    
    return vout;
}