struct VertexOut
{
	float4 Position	: SV_Position;
	float2 texcoord	: TEXCOORD0;
	uint mask  	: TEXCOORD1;
};

Texture2D depthTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D accumulationTexture : register(t3);
Texture2D specularTexture : register(t4);
SamplerState samplerObj : register(s0);

cbuffer CBProjParams : register(b0)
{
	float4 g_ProjParams; // (1.0f / proj[0][0], 1.0f / proj[1][1], proj[3][2], proj[2][2])
};

float DepthFromNDCToViewSpace(float val)
{
	return g_ProjParams.z / (val - g_ProjParams.w);
}

float4 PS(VertexOut vout) : SV_Target
{
	float z = DepthFromNDCToViewSpace(depthTexture.Sample(samplerObj, vout.texcoord).x);
	float4 depth = float4(1.0f - saturate(z / 75.0f), 1.0f - saturate(z / 125.0f), 1.0f - saturate(z / 200.0f), 0.0f);

	float4 diffuse = float4(diffuseTexture.Sample(samplerObj, vout.texcoord).rgb, 0.0f);

	float4 normal = float4(normalTexture.Sample(samplerObj, vout.texcoord).xyz, 0.0f);

	float4 accumulation = float4(accumulationTexture.Sample(samplerObj, vout.texcoord).rgb, 0.0f);

	float4 specular = float4(specularTexture.Sample(samplerObj, vout.texcoord).rgb, 0.0f);

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float mask = (float)(vout.mask & 1);
	finalColor += depth * mask;

	mask = (float)((vout.mask >> 1) & 1);
	finalColor += diffuse * mask;

	mask = (float)((vout.mask >> 2) & 1);
	finalColor += normal * mask;

	mask = (float)((vout.mask >> 3) & 1);
	finalColor += accumulation * mask;

	mask = (float)((vout.mask >> 4) & 1);
	finalColor += specular * mask;

	return finalColor;
}