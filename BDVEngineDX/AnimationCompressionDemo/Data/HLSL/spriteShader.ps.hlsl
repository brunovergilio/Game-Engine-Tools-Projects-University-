Texture2D ObjTexture : register(t0);
SamplerState ObjSampler : register(s0);

struct VertexOut
{
	float4 posH   : SV_POSITION;
	float2 tex    : TEXCOORD0;
	float4 color  : COLOR;
};

float4 PS(VertexOut vout) : SV_Target
{
	return vout.color * ObjTexture.Sample(ObjSampler, vout.tex);
}