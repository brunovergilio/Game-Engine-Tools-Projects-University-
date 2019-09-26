static float2 arrOffsets[6] =
{
	float2(-0.20,  0.30), //  X
	float2(-0.80,  0.30), // -X

	float2(-0.50,  0.70), //  Y
	float2(-0.50, -0.10), // -Y

	float2(-0.50,  0.30), //  Z
	float2( 0.10,  0.30)  // -Z
};

static const float2 arrBasePos[6] =
{
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(1.0, -1.0),

	float2(-1.0, 1.0),
	float2(1.0, -1.0),
	float2(-1.0, -1.0)
};

static const float3 arrUV[36] =
{
	// X
	float3(1.0f, 1.0, 1.0),
	float3(1.0f, 1.0, -1.0),
	float3(1.0f, -1.0, -1.0),

	float3(1.0f, 1.0, 1.0),
	float3(1.0f, -1.0, -1.0),
	float3(1.0f, -1.0, 1.0),

	// -X
	float3(-1.0f, 1.0, -1.0),
	float3(-1.0f, 1.0, 1.0),
	float3(-1.0f, -1.0, 1.0),

	float3(-1.0f, 1.0, -1.0),
	float3(-1.0f, -1.0, 1.0),
	float3(-1.0f, -1.0, -1.0),

	// Y
	float3(-1.0f, 1.0, -1.0),
	float3(1.0f, 1.0, -1.0),
	float3(1.0f, 1.0, 1.0),

	float3(-1.0f, 1.0, -1.0),
	float3(1.0f, 1.0, 1.0),
	float3(-1.0f, 1.0, 1.0),

	// -Y
	float3(-1.0f, -1.0, 1.0),
	float3(1.0f, -1.0, 1.0),
	float3(1.0f, -1.0, -1.0),

	float3(-1.0f, -1.0, -1.0),
	float3(1.0f, -1.0, 1.0),
	float3(-1.0f, -1.0, 1.0),

	// Z
	float3(-1.0f, 1.0, 1.0),
	float3(1.0f, 1.0, 1.0),
	float3(1.0f, -1.0, 1.0),

	float3(-1.0f, 1.0, 1.0),
	float3(1.0f, -1.0, 1.0),
	float3(-1.0f, -1.0, 1.0),

	// -Z
	float3(1.0f, 1.0, -1.0),
	float3(-1.0f, 1.0, -1.0),
	float3(-1.0f, -1.0, -1.0),

	float3(1.0f, 1.0, -1.0),
	float3(-1.0f, -1.0, -1.0),
	float3(1.0f, -1.0, -1.0)
};

struct VertexOut
{
    float4 position	: SV_Position;
    float3 texcoord	: TEXCOORD0;
};

VertexOut VS(uint vertexID : SV_VertexID)
{
    VertexOut vout;

    vout.position = float4(arrBasePos[vertexID % 6].x * 0.15f + arrOffsets[vertexID / 6].x,
		arrBasePos[vertexID % 6].y * 0.2f + arrOffsets[vertexID / 6].y,
		0.0, 1.0);
	vout.texcoord = arrUV[vertexID];

    return vout;
}