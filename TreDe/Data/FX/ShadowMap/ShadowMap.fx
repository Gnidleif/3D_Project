#include "../LightDef.fx"

cbuffer cbEveryFrame
{
	float4x4 sWorld;
	float4x4 sLightVP;
};

cbuffer cbResolution
{
	float gScreenX;
	float gScreenY;
};

struct sVSIn
{
	float3 PosL : POSITION;
};

struct sVSOut
{
	float4 PosH : SV_POSITION;
};

sVSOut sVSScene(sVSIn input)
{
	sVSOut output = (sVSOut)0;
	float4x4 wvp = mul(sWorld, sLightVP);

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
		SetVertexShader(CompileShader(vs_5_0, sVSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);
	}
};