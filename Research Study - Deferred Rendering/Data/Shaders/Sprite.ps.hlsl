cbuffer CBPixelData : register(b0)
{
    float3 gColor;
    int gUseTexture;
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSampler : register(s0);

struct VertexOut
{
	float4 posH   : SV_POSITION;
	float2 tex    : TEXCOORD0;
};

float4 PS(VertexOut vout) : SV_Target
{
	float4 color = float4(gColor, 1.0f);
	if (gUseTexture)
	{
		color = color * ObjTexture.Sample(ObjSampler, vout.tex);	
	}
	return color;
}