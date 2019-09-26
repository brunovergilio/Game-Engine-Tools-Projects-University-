cbuffer CBObject : register(b0)
{
	float4x4 g_WVP;
};

struct VertexIn
{
	float3 position  : POSITION;
};

struct VertexOut
{
    float4 posH      : SV_POSITION;
    float2 posC      : TEXCOORD;
};

static const float2 arrBasePos[4] = {
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

VertexOut VS_ScreenQuad(uint id : SV_VertexId)
{
    VertexOut vout;

	vout.posH = float4(arrBasePos[id].xy, 0.0f, 1.0f);
	vout.posC = vout.posH.xy;

	return vout;
}

VertexOut VS_LightVolume(VertexIn vin)
{
	VertexOut vout;

	vout.posH = mul(g_WVP, float4(vin.position, 1.0f));
	vout.posC = vout.posH.xy / vout.posH.w;

	return vout;
}