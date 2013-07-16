#include "../LightDef.fx"

cbuffer cbPerFrame
{
	float4x4 gView;
	float4x4 gProj;
};

cbuffer cbPerObject
{
	Material gMaterial;
	float4x4 gWorld;
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

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

// Input Vertex shader
struct VSIn
{
	float3 PosL : POSITION; // Local position
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentL : TANGENT;
};

// Input Pixel shader
struct PSIn
{
	float4 PosH : SV_Position; // Homogenous position
	float3 PosW : POSITION; // World position
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentW : TANGENT;
};

// VS -> PS
PSIn VSScene(VSIn input)
{
	PSIn output = (PSIn)0; // If something goes to shit, it'll be here
	float4x4 wvp = mul(gWorld, mul(gView, gProj));

	output.PosH = mul(float4(input.PosL, 1.0f), wvp);

	output.PosW = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	//output.Normal = mul(input.Normal, (float3x3) wvp);
	//output.Normal = normalize(output.Normal);
	output.Normal = mul(input.Normal, (float3x3)gWorldInvTranspose);
	output.TangentW = mul(input.TangentL, gWorld);
	output.TexC = input.TexC;

	return output;
};

float4 PSScene(PSIn input,
			   uniform bool alphaClip) : SV_Target
{
	float4 texColor = gDiffMap.Sample(samLinear, input.TexC);
	float3 normMapSamp = gNormMap.Sample(samLinear, input.TexC).rgb;
	float3 bumpNormal = NormalSampleToWorldSpace(normMapSamp, input.Normal, input.TangentW);

	if(alphaClip) // If alpha clipping is true
		clip(texColor.a - 0.1f);

	return texColor;
};

RasterizerState Wireframe
{
	FillMode = WireFrame;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState Solidframe
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

technique11 NormalMapSolidTech
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(false)));
		SetRasterizerState(Solidframe);
	}
};

technique11 NormalMapSolidAlphaTech
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(true)));
		SetRasterizerState(Solidframe);
	}
};

technique11 NormalMapWireTech
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(false)));
		SetRasterizerState(Wireframe);
	}
};