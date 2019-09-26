#define HEMISPHERIC 0
#define DIRECTIONAL 1
#define POINT 2
#define SPOT 3

//#define DIRECTIONAL 0
//#define POINT 1
//#define SPOT 2

struct Light
{
	// 16 bytes
	float4 m_Color;

	// 16 bytes
	float3 m_Position;
	float m_Range;

	// 16 bytes
	float3 m_Direction;
	uint m_LightType;

	// 16 bytes
	float m_CosInnerCone;
	float m_CosOuterCone;
	float m_Intensity;
	uint m_CastsShadow;

	// 16 bytes
	float3 m_Attenuation;
	float m_Enabled;
};

cbuffer CBLight : register(b0)
{
	Light g_Light;
};

cbuffer CBLightParams : register(b1)
{
	float4 g_ProjParams; // (1.0f / proj[0][0], 1.0f / proj[1][1], proj[3][2], proj[2][2])
	float4x4 g_InvView; // View ^ -1
};

cbuffer CBShadowMap : register(b2)
{
	float4x4 g_ShadowMap;
	float2 g_PointProj;
	float g_ResolutionRCP;
	float g_Pad;
	float4 g_OffsetsX;
	float4 g_OffsetsY;
	float4 g_OffsetsScale;
};

struct Material
{
	float4 m_Diffuse;
	float4 m_Specular;
	float4 m_Accumulation;
};

struct VertexOut
{
	float4 posH      : SV_POSITION;
	float2 posC      : TEXCOORD;
};

Texture2D depthTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D accumulationTexture : register(t3);
Texture2D specularTexture : register(t4);
Texture2D shadowMapTexture : register(t5);
TextureCube shadowMapTextureCube : register(t6);
Texture2DArray cascadeShadowMapTexture : register(t7);
SamplerComparisonState samplerClamp : register(s0);

float DepthFromNDCToViewSpace(float val)
{
	return g_ProjParams.z / (val - g_ProjParams.w);
}

float3 PositionFromDepth(float2 posC, float depth)
{
	return mul(g_InvView, float4(posC.xy * g_ProjParams.xy * depth, depth, 1.0f)).xyz;
}


float CalcDirectionalShadow(float3 position)
{
	// Transform the world position to shadow space
	float4 posShadowSpace = mul(g_ShadowMap, float4(position, 1.0));

	// Transform the shadow space position into each cascade position
	float4 posCascadeSpaceX = (g_OffsetsX + posShadowSpace.xxxx) * g_OffsetsScale;
	float4 posCascadeSpaceY = (g_OffsetsY + posShadowSpace.yyyy) * g_OffsetsScale;

	// Check which cascade we are in
	float4 inCascadeX = abs(posCascadeSpaceX) <= 1.0;
	float4 inCascadeY = abs(posCascadeSpaceY) <= 1.0;
	float4 inCascade = inCascadeX * inCascadeY;

	// Prepare a mask for the highest quality cascade the position is in
	float4 bestCascadeMask = inCascade;
	bestCascadeMask.yzw = (1.0 - bestCascadeMask.x) * bestCascadeMask.yzw;
	bestCascadeMask.zw = (1.0 - bestCascadeMask.y) * bestCascadeMask.zw;
	bestCascadeMask.w = (1.0 - bestCascadeMask.z) * bestCascadeMask.w;
	float bestCascade = dot(bestCascadeMask, float4(0.0, 1.0, 2.0, 3.0));

	// Pick the position in the selected cascade
	float3 UVD;
	UVD.x = dot(posCascadeSpaceX, bestCascadeMask);
	UVD.y = dot(posCascadeSpaceY, bestCascadeMask);
	UVD.z = posShadowSpace.z;

	// Convert to shadow map UV values
	UVD.xy = 0.5 * UVD.xy + 0.5;
	UVD.y = 1.0 - UVD.y;

	const float2 offsets[16] = 
	{
		float2(-1.5f, -1.5f), float2(-0.5f, -1.5f), float2(0.5f, -1.5f), float2(1.5f, -1.5f),
		float2(-1.5f, -0.5f), float2(-0.5f, -0.5f), float2(0.5f, -0.5f), float2(1.5f, -0.5f),
		float2(-1.5f,  0.5f), float2(-0.5f,  0.5f), float2(0.5f,  0.5f), float2(1.5f,  0.5f),
		float2(-1.5f,  1.5f), float2(-0.5f,  1.5f), float2(0.5f,  1.5f), float2(1.5f,  1.5f)
	};

	//float shadow = 0.0f;
	//[unroll]
	//for(int i = 0; i < 16; ++i)
	//{
	//	shadow += cascadeShadowMapTexture.SampleCmpLevelZero(samplerClamp, float3(UVD.xy + offsets[i] * g_ResolutionRCP, bestCascade), UVD.z);
	//}

	// Compute the hardware PCF value
	float shadow = cascadeShadowMapTexture.SampleCmpLevelZero(samplerClamp, float3(UVD.xy, bestCascade), UVD.z); 
	
	// set the shadow to one (fully lit) for positions with no cascade coverage
	//shadow = saturate(shadow + 1.0 - any(bestCascadeMask));
	
	return shadow;
}

float4 CalcDirectionalLight(Material M, float3 P, float3 N)
{
	float4 directionalLight = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Phong diffuse
	float3 L = -g_Light.m_Direction;
	float NDotL = dot(N, L);

	if (NDotL > 0.0f)
	{
		directionalLight = g_Light.m_Color * saturate(NDotL) * g_Light.m_Intensity * M.m_Diffuse;

		// Blinn specular
		float3 V = g_InvView[3].xyz - P;
		V = normalize(V);
		float3 H = normalize(V + L);
		float NDotH = saturate(dot(H, N));
		directionalLight += g_Light.m_Color * pow(NDotH, M.m_Specular.a) * float4(M.m_Specular.rgb, 1.0f) * g_Light.m_Intensity;

		if (g_Light.m_CastsShadow)
		{
			directionalLight.xyz *= CalcDirectionalShadow(P);
		}
	}

	return directionalLight;
}

float CalcPointShadow(float3 direction)
{
	// Get absolute values
	float3 absDir = abs(direction);
	// Get the actual Z Direction - doesn't necessarily have to be the Z component
	float zDir = max(absDir.x, max(absDir.y, absDir.z));
	// Convert to the actual depth on NDC space [0, 1]
	float closestDepth = (g_PointProj.x * zDir + g_PointProj.y) / zDir;

	float offsets[4] = { -1.5f, -0.5f, 0.5f, 1.5f };

	float shadow = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				shadow += shadowMapTextureCube.SampleCmpLevelZero(samplerClamp,
					direction + float3(offsets[i], offsets[j], offsets[k]) * g_ResolutionRCP, closestDepth);
			}
		}
	}

	return shadow / 64.0f;
}

float4 CalcPointLight(Material M, float3 P, float3 N)
{
	float4 pointLight = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 L = g_Light.m_Position - P;
	float LLen = length(L);
	if (LLen <= g_Light.m_Range)
	{
		L /= LLen;
		float NDotL = dot(N, L);
		if (NDotL > 0.0f)
		{
			pointLight = g_Light.m_Color * saturate(NDotL) * g_Light.m_Intensity * M.m_Diffuse;

			// Blinn specular
			float3 V = g_InvView[3].xyz - P;
			V = normalize(V);
			float3 H = normalize(V + L);
			float NDotH = saturate(dot(H, N));
			pointLight += g_Light.m_Color * pow(NDotH, M.m_Specular.a) * float4(M.m_Specular.rgb, 1.0f) * g_Light.m_Intensity;

			float attenuation = 1.0f / dot(g_Light.m_Attenuation, float3(1.0f, LLen, LLen * LLen));

			pointLight.xyz *= attenuation;

			if (g_Light.m_CastsShadow)
			{
				pointLight.xyz *= CalcPointShadow(P - g_Light.m_Position);
			}
		}
	}

	return pointLight;
}

float CalcSpotShadow(float3 position)
{
	float4 posShadowMap = mul(g_ShadowMap, float4(position, 1.0));

	float3 uv = posShadowMap.xyz / posShadowMap.w;

	uv.xy = 0.5 * uv.xy + 0.5;
	uv.y = 1.0 - uv.y;

	const float2 offsets[16] = 
	{
		float2(-1.5f, -1.5f), float2(-0.5f, -1.5f), float2(0.5f, -1.5f), float2(1.5f, -1.5f),
		float2(-1.5f, -0.5f), float2(-0.5f, -0.5f), float2(0.5f, -0.5f), float2(1.5f, -0.5f),
		float2(-1.5f,  0.5f), float2(-0.5f,  0.5f), float2(0.5f,  0.5f), float2(1.5f,  0.5f),
		float2(-1.5f,  1.5f), float2(-0.5f,  1.5f), float2(0.5f,  1.5f), float2(1.5f,  1.5f)
	};

	float shadow = 0.0f;
	[unroll]
	for(int i = 0; i < 16; ++i)
	{
		shadow += shadowMapTexture.SampleCmpLevelZero(samplerClamp, uv.xy + offsets[i] * g_ResolutionRCP, uv.z);
	}

	return shadow /= 16.0f;
}

float4 CalcSpotLight(Material M, float3 P, float3 N)
{
	float4 spotLight = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 L = g_Light.m_Position - P;
	float LLen = length(L);
	L /= LLen;

	float spot = dot(-g_Light.m_Direction, L);
	if (LLen <= g_Light.m_Range && spot >= g_Light.m_CosOuterCone)
	{
		float NDotL = dot(N, L);
		if (NDotL > 0.0f)
		{
			spotLight = g_Light.m_Color * saturate(NDotL) * g_Light.m_Intensity * M.m_Diffuse;

			// Blinn specular
			float3 V = g_InvView[3].xyz - P;
			V = normalize(V);
			float3 H = normalize(V + L);
			float NDotH = saturate(dot(H, N));
			spotLight += g_Light.m_Color * pow(NDotH, M.m_Specular.a) * float4(M.m_Specular.rgb, 1.0f) * g_Light.m_Intensity;

			//float spot = dot(-g_Light.m_Direction, L);
			spot = smoothstep(g_Light.m_CosOuterCone, g_Light.m_CosInnerCone, spot);

			float attenuation = spot / dot(g_Light.m_Attenuation, float3(1.0f, LLen, LLen * LLen));

			spotLight.xyz *= attenuation;

			if (g_Light.m_CastsShadow)
			{
				spotLight.xyz *= CalcSpotShadow(P);
			}
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
	switch (g_Light.m_LightType)
	{
	case HEMISPHERIC:
		finalColor = accumulation;
		break;
	case DIRECTIONAL:
		finalColor = CalcDirectionalLight(material, position, normal);
		break;
	case POINT:
		finalColor = CalcPointLight(material, position, normal);
		break;
	case SPOT:
		finalColor = CalcSpotLight(material, position, normal);
		break;
	}

    return finalColor;
}