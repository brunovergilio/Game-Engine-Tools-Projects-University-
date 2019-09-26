#define HEMISPHERIC 0
#define DIRECTIONAL 1
#define POINT 2
#define SPOT 3

cbuffer CBLightParams : register(b0)
{
	float4 g_ProjParams; // (1.0f / proj[0][0], 1.0f / proj[1][1], proj[3][2], proj[2][2])
	float4x4 g_InvView; // View ^ -1
};

struct Material
{
	float4 m_Diffuse;
	float4 m_Specular;
	float4 m_Accumulation;
};

struct VertexOut
{
	float4 posH          : SV_POSITION;
	float2 posC          : TEXCOORD;
	float4 m_Color       : LIGHTCOLOR;
	float3 m_Position    : LIGHTPOS;
	float m_Range        : RANGE;
	float3 m_Direction   : DIRECTION;
	uint m_LightType     : LIGHTTYPE;
	float m_CosInnerCone : COSINNERCONE;
	float m_CosOuterCone : COSOUTERCONE;
	float m_Intensity    : INTENSITY;
	float m_Pad          : PAD;
	float3 m_Attenuation : ATTENUATION;
	uint m_Enabled       : ENABLED;
};

Texture2D depthTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D accumulationTexture : register(t3);
Texture2D specularTexture : register(t4);

float DepthFromNDCToViewSpace(float val)
{
	return g_ProjParams.z / (val - g_ProjParams.w);
}

float3 PositionFromDepth(float2 posC, float depth)
{
	return mul(g_InvView, float4(posC.xy * g_ProjParams.xy * depth, depth, 1.0f)).xyz;
}

float4 CalcDirectionalLight(VertexOut vout, Material M, float3 P, float3 N)
{
	float4 directionalLight = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Phong diffuse
	float3 L = -vout.m_Direction;
	float NDotL = dot(N, L);

	if (NDotL > 0.0f)
	{
		directionalLight = vout.m_Color * saturate(NDotL) * vout.m_Intensity * M.m_Diffuse;

		// Blinn specular
		float3 V = g_InvView[3].xyz - P;
		V = normalize(V);
		float3 H = normalize(V + L);
		float NDotH = saturate(dot(H, N));
		directionalLight += vout.m_Color * pow(NDotH, M.m_Specular.a) * float4(M.m_Specular.rgb, 1.0f) * vout.m_Intensity;
	}

	return directionalLight;
}

float4 CalcPointLight(VertexOut vout, Material M, float3 P, float3 N)
{
	float4 pointLight = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 L = vout.m_Position - P;
	float LLen = length(L);
	if (LLen <= vout.m_Range)
	{
		L /= LLen;
		float NDotL = dot(N, L);
		if (NDotL > 0.0f)
		{
			pointLight = vout.m_Color * saturate(NDotL) * vout.m_Intensity * M.m_Diffuse;

			// Blinn specular
			float3 V = g_InvView[3].xyz - P;
			V = normalize(V);
			float3 H = normalize(V + L);
			float NDotH = saturate(dot(H, N));
			pointLight += vout.m_Color * pow(NDotH, M.m_Specular.a) * float4(M.m_Specular.rgb, 1.0f) * vout.m_Intensity;

			float attenuation = 1.0f / dot(vout.m_Attenuation, float3(1.0f, LLen, LLen * LLen));

			pointLight.xyz *= attenuation;
		}
	}

	return pointLight;
}

float4 CalcSpotLight(VertexOut vout, Material M, float3 P, float3 N)
{
	float4 spotLight = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 L = vout.m_Position - P;
	float LLen = length(L);
	L /= LLen;

	float spot = dot(-vout.m_Direction, L);
	if (LLen <= vout.m_Range && spot >= vout.m_CosOuterCone)
	{
		float NDotL = dot(N, L);
		if (NDotL > 0.0f)
		{
			spotLight = vout.m_Color * saturate(NDotL) * vout.m_Intensity * M.m_Diffuse;

			// Blinn specular
			float3 V = g_InvView[3].xyz - P;
			V = normalize(V);
			float3 H = normalize(V + L);
			float NDotH = saturate(dot(H, N));
			spotLight += vout.m_Color * pow(NDotH, M.m_Specular.a) * float4(M.m_Specular.rgb, 1.0f) * vout.m_Intensity;

			spot = smoothstep(vout.m_CosOuterCone, vout.m_CosInnerCone, spot);

			float attenuation = spot / dot(vout.m_Attenuation, float3(1.0f, LLen, LLen * LLen));

			spotLight.xyz *= attenuation;
		}
	}

	return spotLight;
}

float4 PS(VertexOut vout) : SV_Target
{
	int3 index = int3(vout.posH.xy, 0);

	float depth = DepthFromNDCToViewSpace(depthTexture.Load(index).x);
	float3 position = PositionFromDepth(vout.posC, depth);
	
	float4 diffuse = diffuseTexture.Load(index);
	
	float3 normal = normalTexture.Load(index).xyz;
	normal = normalize(normal * 2.0f - 1.0f);

	float4 accumulation = accumulationTexture.Load(index);

	float4 specular = specularTexture.Load(index);
	specular.a = exp2(specular.a * 10.5f); // Encoding by Michiel van der Leeuw - Killzone 2

	Material material;
	material.m_Diffuse = diffuse;
	material.m_Specular = specular;
	material.m_Accumulation = accumulation;

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	[flatten]
	switch (vout.m_LightType)
	{
	case HEMISPHERIC:
		finalColor = accumulation;
		break;
	case DIRECTIONAL:
		finalColor = CalcDirectionalLight(vout, material, position, normal);
		break;
	case POINT:
		finalColor = CalcPointLight(vout, material, position, normal);
		break;
	case SPOT:
		finalColor = CalcSpotLight(vout, material, position, normal);
		break;
	}

    return finalColor;
}