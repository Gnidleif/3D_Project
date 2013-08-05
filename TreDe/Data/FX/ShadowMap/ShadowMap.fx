#include "../LightDef.fx"

cbuffer cbPerFrame
{
	float3 gEyePos;
	float gHeightScale;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gView;
	float4x4 gProj;
	float4x4 gWorldInvTranspose;
};

Texture2D gDiffMap;
Texture2D gNormMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct ShadowVSIn
{
	float3 PosL : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentL : TANGENT;
};

struct ShadowVSOut
{
	float4 PosH : SV_Position;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentW : TANGENT;
};

ShadowVSOut ShadowVSScene(ShadowVSIn input)
{
	ShadowVSOut output = (ShadowVSOut)0;

	float4x4 vp = mul(gView, gProj);

	output.PosH = mul(float4(input.PosL, 1.0f), vp);
	output.PosW = mul(float4(input.PosL, 1.0f), gWorld).xyz;

	output.Normal = mul(input.Normal, (float3x3) gWorldInvTranspose);
	output.Normal = normalize(output.Normal);

	output.TexC = input.TexC;

	output.TangentW = mul(input.TangentL, gWorld);
	// If shit fucks up, it WILL be here, please don't look anywhere else.
	const float interval = 20.0f;
	float mipLevel = clamp((distance(output.PosW, gEyePos) - interval) / interval, 0.0f, 6.0f);

	// Sample the height of the map, which is stored in the alpha-channel
	float height = gNormMap.SampleLevel(samLinear, output.TexC, mipLevel).a;

	output.PosW += (gHeightScale*(height-1.0f))*output.Normal;

	return output;
}

// Only used to alphaclip the shadowmap
void ShadowPSScene(ShadowVSOut input)
{
	float4 diffuse = gDiffMap.Sample(samLinear, input.TexC);
	clip(diffuse.a - 0.15f);
}

RasterizerState Depth
{
	DepthBias = 10000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

technique11 Build
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, ShadowVSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(Depth);
	}
};

technique11 BuildAlpha
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, ShadowVSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ShadowPSScene()));
		
		SetRasterizerState(Depth);
	}
};