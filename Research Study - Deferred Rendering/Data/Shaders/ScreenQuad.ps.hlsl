struct VertexOut
{
	float4 posH	: SV_Position;
	float2 posC	: TEXCOORD0;
};

Texture2D screenTexture : register(t0);
SamplerState samplerObj : register(s0);

float4 PS(VertexOut vout) : SV_Target
{
	return screenTexture.Sample(samplerObj, vout.posC);
}