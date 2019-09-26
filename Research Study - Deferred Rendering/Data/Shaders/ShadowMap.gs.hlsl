cbuffer CBShadowParams : register(b0)
{
	float4x4 g_ViewProj[6];
};

struct GeometryOut
{
	float4 pos   : SV_POSITION;
	uint rtIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GS(triangle float4 posL[3] : SV_Position, inout TriangleStream<GeometryOut> outStream)
{
	for(int i = 0; i < 6; i++ )
	{
		GeometryOut gout;
		gout.rtIndex = i;
		for(int v = 0; v < 3; v++ )
		{
			gout.pos = mul(g_ViewProj[i], posL[v]);
			outStream.Append(gout);
		}
		outStream.RestartStrip();
	}
}