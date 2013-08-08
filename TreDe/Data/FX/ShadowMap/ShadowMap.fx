cbuffer cbEveryFrame
{
	float4x4 gWorld;
	float4x4 gLightVP;
};

struct VSIn
{
	float3 PosL : POSITION;
};

struct VSOut
{
	float4 PosH : SV_POSITION;
};

VSOut VSScene(VSIn input)
{
	VSOut output = (VSOut)0;
	float4x4 wvp = mul(gWorld, gLightVP);

	output.PosH = mul(float4(input.PosL, 1.0f), wvp);

	return output;
}

RasterizerState Backface
{
	CullMode = Back;
};

technique11 ShadowTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(Backface);
	}
};