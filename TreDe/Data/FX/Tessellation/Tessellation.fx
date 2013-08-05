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

Texture2D gDiffMap;
Texture2D gNormMap;

cbuffer cbFixed
{
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
	float4 TangentL : TANGENT;
};

struct VSOut
{
	float3 PosL : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentL : TANGENT;
};

struct HSOut
{
	float3 PosL : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentL : TANGENT;
};

struct DSOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentW : TANGENT;
};

VSOut VSScene(VSIn input)
{
	VSOut output = (VSOut)0;

	output.PosL = input.PosL;
	output.Normal = input.Normal;
	output.TexC = input.TexC;
	output.TangentL = input.TangentL;

	return output;
}

struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

PatchTess ConstHS(InputPatch<VSOut, 3> patch,
				  uint patchID : SV_PrimitiveID)
{
	PatchTess output = (PatchTess)0;

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
	HSOut output = (HSOut)0;

	output.PosL = patch[i].PosL;
	output.Normal = patch[i].Normal;
	output.TexC = patch[i].TexC;
	output.TangentL = patch[i].TangentL;

	return output;
}

[domain("tri")]
DSOut DSScene(PatchTess patchTess,
			  float3 coords : SV_DomainLocation,
			  const OutputPatch<HSOut, 3> tri)
{
	DSOut output = (DSOut)0;

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

	output.TexC = texC;

	float4 tangent =
		coords.x * tri[0].TangentL +
		coords.y * tri[1].TangentL +
		coords.z * tri[2].TangentL;

	output.TangentW = mul(tangent, gWorld);

	return output;
}

float4 PSScene(DSOut input,
			   uniform bool alphaClip,
			   uniform bool useTex) : SV_Target
{
	input.Normal = normalize(input.Normal);

	float4 texColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	if(useTex)
	{
		texColor = gDiffMap.Sample(samLinear, input.TexC);

		if(alphaClip) // If alpha clipping is true
			clip(texColor.a - 0.1f);
	}

	return texColor;
};

float4 PSScene_Lights(DSOut input,
			   uniform bool alphaClip,
			   uniform bool useTex,
			   uniform int dirLightAmount,
			   uniform int pointLightAmount,
			   uniform int spotLightAmount) : SV_TARGET
{
	input.Normal = normalize(input.Normal);
	float4 texColor = float4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow for tessellation-wire
	if(useTex)
	{
		texColor = gDiffMap.Sample(samLinear, input.TexC);
	}

	float3 normMapSamp = gNormMap.Sample(samLinear, input.TexC).rgb;
	float3 bumpNormal = NormalSampleToWorldSpace(normMapSamp, input.Normal, input.TangentW);

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
			ComputeDirectionalLight(gMaterial, gDirLights[i], bumpNormal, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		[unroll]
		for(int j = 0; j < pointLightAmount; ++j)
		{
			float4 A, D, S;
			ComputePointLight(gMaterial, gPointLights[j], input.PosW, bumpNormal, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		[unroll]
		for(int k = 0; k < spotLightAmount; ++k)
		{
			float4 A, D, S;
			ComputeSpotLight(gMaterial, gSpotLights[k], input.PosW, bumpNormal, toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		litColor = texColor * (ambient + diffuse) + specular;
		litColor += texColor * lightAddScale;
	}

	if(alphaClip) // If alpha clipping is true
		clip(texColor.a - 0.1f);

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
		SetPixelShader(CompileShader(ps_5_0, PSScene(false, true)));

		SetRasterizerState(Solidframe);
	}
}

technique11 SolidAlpha
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VSScene()));
		SetHullShader(CompileShader(hs_5_0, HSScene()));
		SetDomainShader(CompileShader(ds_5_0, DSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSScene(true, true)));

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
		SetPixelShader(CompileShader(ps_5_0, PSScene(false, false)));

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
		SetPixelShader(CompileShader(ps_5_0, PSScene_Lights(false, true, 0, 2, 0)));

		SetRasterizerState(Solidframe);
	}
};

technique11 AllLightsAlpha
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VSScene()));
		SetHullShader(CompileShader(hs_5_0, HSScene()));
		SetDomainShader(CompileShader(ds_5_0, DSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSScene_Lights(true, true, 0, 2, 0)));

		SetRasterizerState(Solidframe);
	}
};