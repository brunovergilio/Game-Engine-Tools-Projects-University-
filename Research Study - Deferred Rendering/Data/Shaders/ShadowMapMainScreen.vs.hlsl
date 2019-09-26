static float2 arrOffset = float2(-0.80, 0.75);

static const float2 arrBasePos[4] =
{
	float2(1.0, 1.0),
	float2(1.0, -1.0),
	float2(-1.0, 1.0),
	float2(-1.0, -1.0),
};

static const float2 arrUV[4] =
{
	float2(1.0, 0.0),
	float2(1.0, 1.0),
	float2(0.0, 0.0),
	float2(0.0, 1.0),
};

struct VertexOut
{
    float4 Position	: SV_Position;
    float2 texcoord	: TEXCOORD0;
};

VertexOut VS( uint VertexID : SV_VertexID )
{
    VertexOut vout;

    vout.Position = float4(arrBasePos[VertexID % 4].x * 0.15f + arrOffset.x,
		arrBasePos[VertexID % 4].y * 0.2f + arrOffset.y,
		0.0, 1.0);
    vout.texcoord = arrUV[VertexID % 4].xy;
    
    return vout;
}