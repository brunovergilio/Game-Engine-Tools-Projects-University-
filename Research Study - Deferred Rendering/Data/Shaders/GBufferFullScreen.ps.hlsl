struct VertexOut
{
	float4 Position	: SV_Position;
	float2 texcoord	: TEXCOORD0;
};

Texture2D gbufferTexture : register(t0);
SamplerState samplerObj : register(s0);

cbuffer CBProjParams : register(b0)
{
	float4 g_ProjParams; // (0, 0, proj[3][2], proj[2][2])
};

float DepthFromNDCToViewSpace(float val)
{
	return g_ProjParams.z / (val - g_ProjParams.w);
}

float4 PS(VertexOut vout) : SV_Target
{
	return float4(gbufferTexture.Sample(samplerObj, vout.texcoord).rgb, 1.0f);
}

float4 PS_Depth(VertexOut vout) : SV_Target
{
	float z = DepthFromNDCToViewSpace(gbufferTexture.Sample(samplerObj, vout.texcoord).x);
	return float4(1.0f - saturate(z / 75.0f), 1.0f - saturate(z / 125.0f), 1.0f - saturate(z / 200.0f), 1.0f);
}