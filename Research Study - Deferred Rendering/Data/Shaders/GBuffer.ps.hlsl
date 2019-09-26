struct Material
{
	float4 m_HemsTopColor;
	float4 m_HemsBottomColor;
	float4 m_Diffuse;
	float4 m_Specular;
	int m_UseColor;
	int m_UseTexture;
	float2 m_Pad;
};

cbuffer CBMaterial : register(b0)
{
	Material g_Material;
};

struct VertexOut
{
	float4 posH      : SV_POSITION;
	float2 tex       : TEXCOORD;
	float3 posW      : POSITION;
	float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
};

struct PixelOut
{
	float4 diffuse      : SV_Target0;
	float4 normal       : SV_Target1;
	float4 accumulation : SV_Target2;
	float4 specular     : SV_Target3;
};

Texture2D diffuseTexture : register(t0);
SamplerState samplerObj : register(s0);

float4 CalcHemisphericLight(float4 diffuse, float3 normal)
{
	// normal.y has to be in [0, 1] range
	float up = normal.y;

	float4 ambient = g_Material.m_HemsBottomColor + up * (g_Material.m_HemsTopColor - g_Material.m_HemsBottomColor);

	return ambient * diffuse;
}

PixelOut PS(VertexOut vout)
{
	float3 normal = normalize(vout.normal);

	PixelOut pout;
	pout.diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (g_Material.m_UseColor == 1)
	{
		pout.diffuse *= g_Material.m_Diffuse;
	}
	if (g_Material.m_UseTexture == 1)
	{
		pout.diffuse *= diffuseTexture.Sample(samplerObj, vout.tex);
	}
	normal = normal * 0.5f + 0.5f;
	pout.normal = float4(normal, 1.0f);
	pout.accumulation = CalcHemisphericLight(pout.diffuse, normal);
	pout.specular = float4(g_Material.m_Specular.rgb, clamp(g_Material.m_Specular.a, 0.1f, 255.0f));
	pout.specular.a = log2(pout.specular.a) / 10.5f; // Encoding by Michiel van der Leeuw - Killzone 2

    return pout;
}