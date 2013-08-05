#include "../LightDef.fx"
// Hardcoded values, vill figure out how to change this later :D

//static const float ShadowX = 1024.0f;
//static const float ShadowY = 768.0f;

cbuffer cbEveryFrame
{
	float3 gLightPos;
	float4x4 gShadowWorld;
	float4x4 gShadowView;
	float4x4 gShadowProj;

	uint gResX;
	uint gResY;
};

Texture2D gShadowMap;

SamplerState samShadow
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

SamplerComparisonState samComp
{
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;

	ComparisonFunc = LESS_EQUAL;
};

struct ShadowVSIn
{
	float3 PosL : POSITION;
};


struct ShadowVSOut
{
	float4 PosH : SV_POSITION;
};

ShadowVSOut ShadowVSScene(ShadowVSIn input)
{
	ShadowVSOut output = (ShadowVSOut)0;

	float4x4 wvp = mul(gShadowWorld, mul(gShadowView, gShadowProj));
	output.PosH = mul(float4(input.PosL, 1.0f), wvp);

	return output;
}

float4 ShadowPSScene(ShadowVSOut input) : SV_Target
{
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

RasterizerState Backface
{
	CullMode = Back;
};

technique11 ShadowTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, ShadowVSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ShadowPSScene()));

		SetRasterizerState(Backface);
	}
};