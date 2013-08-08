#include "../LightDef.fx"

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

SamplerComparisonState samComp
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.4f, 0.4f, 0.4f, 0.4f);

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

RasterizerState Backface
{
	CullMode = Back;
};

RasterizerState Depth
{
	DepthBias = 100000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

float2 texOffset(int x, int y)
{
	return float2(x * 1.0f/gResX, y * 1.0f/gResY);
}

float CalcShadowFactor(	SamplerComparisonState compState,
						Texture2D shadowMap,
						float4 shadowPosH)
{
	// Check how to do this later
}

technique11 ShadowTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, ShadowVSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(Depth);
	}
};