cbuffer CBPixelData : register(b0)
{
    float3 gColor;
    int gUseTexture;
    float4 gPerspectiveValues;
};

#define EFFECT_DEPTH 2

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
	if (gUseTexture > 0)
	{
		if (gUseTexture == EFFECT_DEPTH)
		{
			float depth = DepthTexture.Load(location3).x;
			depth = PerspectiveValues.z / (depth + PerspectiveValues.w);
		}
		else
		{
			color = color * ObjTexture.Sample(ObjSampler, vout.tex);
		}
	}
	return color;
}