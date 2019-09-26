struct VertexOut
{
    float4 posH      : SV_POSITION;
    float2 posC      : TEXCOORD;
};

static const float2 arrBasePos[4] = {
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

VertexOut VS(uint id : SV_VertexId)
{
    VertexOut vout;

	vout.posH = float4(arrBasePos[id].xy, 0.0f, 1.0f);
	vout.posC = vout.posH.xy;

	return vout;
}