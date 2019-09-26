struct VertexIn
{
	float3 position    : POSITION;

	float4x4 wvp       : WVP;

	float4 color       : LIGHTCOLOR;
	float3 lightPos    : LIGHTPOS;
	float range        : RANGE;
	float3 direction   : DIRECTION;
	uint lightType     : LIGHTTYPE;
	float cosInnerCone : COSINNERCONE;
	float cosOuterCone : COSOUTERCONE;
	float intensity    : INTENSITY;
	float pad          : PAD;
	float3 attenuation : ATTENUATION;
	uint enabled       : ENABLED;
};

struct VertexOut
{
    float4 posH      : SV_POSITION;
    float2 posC      : TEXCOORD;

	float4 color       : LIGHTCOLOR;
	float3 lightPos    : LIGHTPOS;
	float range        : RANGE;
	float3 direction   : DIRECTION;
	uint lightType     : LIGHTTYPE;
	float cosInnerCone : COSINNERCONE;
	float cosOuterCone : COSOUTERCONE;
	float intensity    : INTENSITY;
	float pad          : PAD;
	float3 attenuation : ATTENUATION;
	uint enabled       : ENABLED;
};

VertexOut VS_LightVolume(VertexIn vin)
{
	VertexOut vout = (VertexOut)0;

	vout.posH = mul(vin.wvp, float4(vin.position, 1.0f));
	vout.posC = vout.posH.xy / vout.posH.w;

	vout.color = vin.color;
	vout.lightPos = vin.lightPos;
	vout.range = vin.range;
	vout.direction = vin.direction;
	vout.lightType = vin.lightType;
	vout.cosInnerCone = vin.cosInnerCone;
	vout.cosOuterCone = vin.cosOuterCone;
	vout.intensity = vin.intensity;
	vout.attenuation = vin.attenuation;

	return vout;
}