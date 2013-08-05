// Tessellation stuff!

#include "../LightDef.fx"

cbuffer cbPerFrame
{
	float gMinDist;
	float gMaxDist;
	float gMinTess;
	float gMaxTess;

	DirectionalLight gDirLights[1];
	PointLight gPointLights[2];
	SpotLight gSpotLights[2];
	float3 gEyePos;
};

cbuffer cbPerObject
{
	Material gMaterial;
	float4x4 gWorld;
	float4x4 gView;
	float4x4 gProj;
	float4x4 gWorldInvTranspose;
};

Texture2D gBlendMap;
Texture2D gTex0;
Texture2D gTex1;
Texture2D gTex2;
Texture2D gTex3;

cbuffer cbFixed
{
	float texScale = 15.0f;
	float lightAddScale = 0.5f;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

float CalcTess(float3 patch)
{
	float dist = distance(patch, gEyePos);
	float sat = saturate((dist - gMinDist) / (gMaxDist - gMinDist));
	return pow(2, (lerp(gMaxTess, gMinTess, sat)));
}

struct VSIn
{
	float3 PosL : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
};

struct VSOut
{
	float3 PosL : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
};

struct HSOut
{
	float3 PosL : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
};

struct DSOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 Tiled : TEXCOORD0; // Moving the calculation of this to the DomainShader, the lazy VertexShader deserves some rest
	float2 Stretched : TEXCOORD1;
};

VSOut VSScene(VSIn input)
{
	VSOut output = (VSOut)0;

	output.PosL = input.PosL;
	output.Normal = input.Normal;
	output.TexC = input.TexC;

	return output;
}
// All the same here, nothing to see!
struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

PatchTess ConstHS(InputPatch<VSOut, 3> patch,
				  uint patchID : SV_PrimitiveID)
{
	PatchTess output;

	float3 centerL = 0.333f*(patch[0].PosL + patch[1].PosL + patch[2].PosL);
	float3 centerW = mul(float4(centerL, 1.0f), gWorld).xyz;

	float tessellation = CalcTess(centerW);

	output.EdgeTess[0] = tessellation;
	output.EdgeTess[1] = tessellation;
	output.EdgeTess[2] = tessellation;

	output.InsideTess = tessellation;

	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstHS")]
//[maxtessfactor(64.0f)]
HSOut HSScene(InputPatch<VSOut, 3> patch,
			  uint i : SV_OutputControlPointID,
			  uint patchID : SV_PrimitiveID)
{
	HSOut output;

	output.PosL = patch[i].PosL;
	output.Normal = patch[i].Normal;
	output.TexC = patch[i].TexC;

	return output;
}

[domain("tri")]
DSOut DSScene(PatchTess patchTess,
			  float3 coords : SV_DomainLocation,
			  const OutputPatch<HSOut, 3> tri)
{
	DSOut output;

	float3 pos =
		coords.x * tri[0].PosL +
		coords.y * tri[1].PosL +
		coords.z * tri[2].PosL;

	output.PosW = mul(float4(pos, 1.0f), gWorld).xyz;

	float4x4 wvp = mul(gWorld, mul(gView, gProj));

	output.PosH = mul(float4(pos, 1.0f), wvp);

	float3 normal =
		coords.x * tri[0].Normal +
		coords.y * tri[1].Normal +
		coords.z * tri[2].Normal;

	output.Normal = mul(normal, (float3x3) gWorldInvTranspose);

	float2 texC =
		coords.x * tri[0].TexC +
		coords.y * tri[1].TexC +
		coords.z * tri[2].TexC;
	// Now this is where shit gets interesting.. Let's see here
	output.Tiled = texScale * texC;

	output.Stretched = texC; // If this is all there's to it, this was barely fun

	return output;
}

float4 PSScene(DSOut input,
			   uniform bool useTex) : SV_Target
{
	input.Normal = normalize(input.Normal);

	float4 texColor = float4(1.0f, 0.0f, 1.0f, 1.0f); // I think this is a cool color.. I guess. Maybe.
	if(useTex)
	{
		float4 c0 = gTex0.Sample(samLinear, input.Tiled);
		float4 c1 = gTex1.Sample(samLinear, input.Tiled);
		float4 c2 = gTex2.Sample(samLinear, input.Tiled);
		float4 c3 = gTex3.Sample(samLinear, input.Tiled);

		float4 blend = gBlendMap.Sample(samLinear, input.Stretched);

		texColor = c0;
		texColor = lerp(texColor, c1, blend.r);
		texColor = lerp(texColor, c2, blend.g);
		texColor = lerp(texColor, c3, blend.b);
	}

	return texColor;
};

float4 PSScene_Lights(DSOut input,
			   uniform bool useTex,
			   uniform int dirLightAmount,
			   uniform int pointLightAmount,
			   uniform int spotLightAmount) : SV_TARGET
{
	input.Normal = normalize(input.Normal);
	float4 texColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
	if(useTex)
	{
		float4 c0 = gTex0.Sample(samLinear, input.Tiled);
		float4 c1 = gTex1.Sample(samLinear, input.Tiled);
		float4 c2 = gTex2.Sample(samLinear, input.Tiled);
		float4 c3 = gTex3.Sample(samLinear, input.Tiled);

		float4 blend = gBlendMap.Sample(samLinear, input.Stretched);

		texColor = c0;
		texColor = lerp(texColor, c1, blend.r);
		texColor = lerp(texColor, c2, blend.g);
		texColor = lerp(texColor, c3, blend.b);
	}

	// Lighting!

	float4 litColor = texColor;
	if(dirLightAmount > 0 || pointLightAmount > 0 || spotLightAmount > 0)
	{
		// This might be wrong, check later
		float3 toEye = normalize(gEyePos - input.PosW);

		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

		[unroll]
		for(int i = 0; i < dirLightAmount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], input.Normal, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		[unroll]
		for(int j = 0; j < pointLightAmount; ++j)
		{
			float4 A, D, S;
			ComputePointLight(gMaterial, gPointLights[j], input.PosW, input.Normal, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		[unroll]
		for(int k = 0; k < spotLightAmount; ++k)
		{
			float4 A, D, S;
			ComputeSpotLight(gMaterial, gSpotLights[k], input.PosW, input.Normal, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		litColor = texColor * (ambient + diffuse) + specular;
		litColor += texColor * lightAddScale;
	}

	// Materials later
	litColor.a = gMaterial.Ambient.a * texColor.a;
	return litColor;
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
	CullMode = Back;
	FrontCounterClockwise = false;
};

technique11 Solid
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VSScene()));
		SetHullShader(CompileShader(hs_5_0, HSScene()));
		SetDomainShader(CompileShader(ds_5_0, DSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSScene(true)));

		SetRasterizerState(Solidframe);
	}
}

technique11 Wire
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VSScene()));
		SetHullShader(CompileShader(hs_5_0, HSScene()));
		SetDomainShader(CompileShader(ds_5_0, DSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSScene(false)));

		SetRasterizerState(Wireframe);
	}
};

technique11 AllLights
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VSScene()));
		SetHullShader(CompileShader(hs_5_0, HSScene()));
		SetDomainShader(CompileShader(ds_5_0, DSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSScene_Lights(true, 0, 2, 0)));

		SetRasterizerState(Solidframe);
	}
};