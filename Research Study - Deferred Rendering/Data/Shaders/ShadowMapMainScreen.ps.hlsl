struct VertexOut
{
	float4 Position	: SV_Position;
	float2 texcoord	: TEXCOORD0;
};

Texture2D shadowMapTexture : register(t0);
SamplerState samplerTrilinear : register(s0);

float4 PS(VertexOut vout) : SV_Target
{
	float val = shadowMapTexture.Sample(samplerTrilinear, vout.texcoord).r;
	return float4(val, val, val, 1.0f);
}