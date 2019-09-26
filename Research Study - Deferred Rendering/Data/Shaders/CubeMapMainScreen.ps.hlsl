struct VertexOut
{
	float4 position	: SV_Position;
	float3 texcoord	: TEXCOORD0;
};

TextureCube shadowMapTextureCube : register(t0);
SamplerState samplerTrilinear : register(s0);

float4 PS(VertexOut vout) : SV_Target
{
	return shadowMapTextureCube.Sample(samplerTrilinear, vout.texcoord);
}