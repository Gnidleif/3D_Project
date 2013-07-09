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

cbuffer cbFixed
{
	float texScale = 15.0f;
};

Texture2D gBlendMap;
Texture2D gTex0;
Texture2D gTex1;
Texture2D gTex2;
Texture2D gTex3;

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
};

// Input Pixel shader
struct PSIn
{
	float4 PosH : SV_Position; // Homogenous position
	float3 PosW : POSITION; // World position
	float3 Normal : NORMAL;
	float2 Tiled : TEXCOORD0;
	float2 Stretched : TEXCOORD1;
};

// VS -> PS
PSIn VSScene(VSIn input)
{
	PSIn output = (PSIn)0; // If something goes to shit, it'll be here
	float4x4 wvp = mul(gWorld, mul(gView, gProj));

	output.PosH = mul(float4(input.PosL, 1.0f), wvp);

	//output.PosH = mul(float4(input.PosL, 1.0f), gWVP);

	output.PosW = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	//output.Normal = mul(input.Normal, (float3x3) wvp);
	//output.Normal = normalize(output.Normal);
	output.Normal = mul(input.Normal, (float3x3)gWorldInvTranspose);
	output.Normal = normalize(output.Normal);

	output.Tiled = texScale * input.TexC;
	output.Stretched = input.TexC;

	return output;
};

float4 PSScene(PSIn input) : SV_Target
{
	//samLinear
	float4 c0 = gTex0.Sample(samLinear, input.Tiled);
	float4 c1 = gTex1.Sample(samLinear, input.Tiled);
	float4 c2 = gTex2.Sample(samLinear, input.Tiled);
	float4 c3 = gTex3.Sample(samLinear, input.Tiled);

	float4 blend = gBlendMap.Sample(samLinear, input.Stretched);

	float4 texColor = c0;
	texColor = lerp(texColor, c1, blend.r);
	texColor = lerp(texColor, c2, blend.g);
	texColor = lerp(texColor, c3, blend.b);

	// Materials later
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

DepthStencilState NoDepthWrites
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};

technique11 TerrainSolidTech
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene()));
		SetRasterizerState(Solidframe);
	}
};

technique11 TerrainWireTech
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene()));
		SetRasterizerState(Wireframe);
		SetDepthStencilState(NoDepthWrites, 0);
	}
};