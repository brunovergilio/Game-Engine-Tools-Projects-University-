#define MAX_CASCADES 4

cbuffer CBCascadeParams : register(b0)
{
	float4x4 g_CascadeMatrices[MAX_CASCADES];
};

struct GeometryOut
{
	float4 pos   : SV_POSITION;
	uint rtIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(12)]
void GS(triangle float4 posL[3] : SV_Position, inout TriangleStream<GeometryOut> outStream)
{
	for(int i = 0; i < MAX_CASCADES; i++ )
	{
		GeometryOut gout;
		gout.rtIndex = i;
		for(int v = 0; v < 3; v++ )
		{
			gout.pos = mul(g_CascadeMatrices[i], posL[v]);
			outStream.Append(gout);
		}
		outStream.RestartStrip();
	}
}