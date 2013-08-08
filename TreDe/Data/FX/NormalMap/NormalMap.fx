#include "../LightDef.fx"

cbuffer cbPerFrame
{
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
	float4x4 gLightVP;
};

cbuffer cbSkin
{
	float4x4 gBoneTransforms[96];
};

cbuffer cbFixed
{
	float lightAddScale = 0.5f;
};

Texture2D gDiffMap;
Texture2D gNormMap;
Texture2D gShadowMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samClampLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

// Input Vertex shader
struct VSIn
{
	float3 PosL : POSITION; // Local position
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentL : TANGENT;
};

struct SkinVSIn
{
	float3 PosL : POSITION; // Local position
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentL : TANGENT;
	float3 Weights : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

// Input Pixel shader
struct VSOut
{
	float4 PosH : SV_POSITION; // Homogenous position
	float3 PosW : POSITION; // World position
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
	float4 TangentW : TANGENT;
};

// VS -> PS
VSOut VSScene(VSIn input)
{
	VSOut output = (VSOut)0; // If something goes to shit, it'll be here

	float4x4 wvp = mul(gWorld, mul(gView, gProj));

	output.PosH = mul(float4(input.PosL, 1.0f), wvp);

	output.PosW = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	output.Normal = mul(input.Normal, (float3x3) gWorld);
	output.TangentW = mul(input.TangentL, gWorld);
	output.TexC = input.TexC;

	return output;
};

VSOut SkinVSScene(SkinVSIn input)
{
	VSOut output = (VSOut)0;

	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	weights[0] = input.Weights.x;
	weights[1] = input.Weights.y;
	weights[2] = input.Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 pos = float3(0.0f, 0.0f, 0.0f);
	float3 normal = float3(0.0f, 0.0f, 0.0f);
	float3 tangent = float3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < 4; ++i)
	{
		pos += weights[i] * mul(float4(input.PosL, 1.0f), gBoneTransforms[input.BoneIndices[i]]).xyz;
		normal += weights[i] * mul(input.Normal, (float3x3) gBoneTransforms[input.BoneIndices[i]]);
		tangent += weights[i] * mul(input.TangentL.xyz, (float3x3) gBoneTransforms[input.BoneIndices[i]]);
	}

	float4x4 wvp = mul(gWorld, mul(gView, gProj));

	output.PosH = mul(float4(pos, 1.0f), wvp);

	output.PosW = mul(float4(pos, 1.0f), gWorld).xyz;
	//output.Normal = mul(input.Normal, (float3x3) wvp);
	//output.Normal = normalize(output.Normal);
	output.Normal = mul(normal, (float3x3)gWorldInvTranspose);
	output.TangentW = float4(mul(tangent, (float3x3) gWorldInvTranspose), input.TangentL.w);
	output.TexC = input.TexC;

	return output;
}

// Shadow map stuff
struct VSOut_Shadow
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD0;
	float4 TangentW : TANGENT;
	float4 ProjTex : TEXCOORD1;
};

VSOut_Shadow VSScene_Shadow(VSIn input)
{
	VSOut_Shadow output = (VSOut_Shadow)0;

	float4x4 wvp = mul(gWorld, mul(gView, gProj));
	float4x4 lightwvp = mul(gWorld, gLightVP);

	output.PosH = mul(float4(input.PosL, 1.0f), wvp);

	output.PosW = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	output.Normal = mul(input.Normal, (float3x3) gWorld);
	output.TangentW = mul(input.TangentL, gWorld);
	output.TexC = input.TexC;

	output.ProjTex = mul(float4(input.PosL, 1.0f), lightwvp);

	return output;
}

float4 PSScene_Shadow(VSOut_Shadow input,
			   uniform bool alphaClip,
			   uniform bool useTex,
			   uniform int dirLightAmount,
			   uniform int pointLightAmount,
			   uniform int spotLightAmount) : SV_Target
{
	input.Normal = normalize(input.Normal);
	float4 texColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	if(useTex)
	{
		texColor = gDiffMap.Sample(samClampLinear, input.TexC);
	}

	float3 normMapSamp = gNormMap.Sample(samClampLinear, input.TexC).rgb;
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
		float shadow = CalcShadow(samClampLinear, gShadowMap, input.ProjTex);

		litColor = (texColor * (ambient + diffuse) + specular) * shadow;
		litColor += texColor * lightAddScale;
	}

	if(alphaClip) // If alpha clipping is true
		clip(texColor.a - 0.1f);

	litColor.a = gMaterial.Diffuse.a * texColor.a;
	return litColor;
};

//

float4 PSScene(VSOut input,
			   uniform bool alphaClip,
			   uniform bool useTex) : SV_Target
{
	input.Normal = normalize(input.Normal);

	float4 texColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	if(useTex)
	{
		texColor = gDiffMap.Sample(samLinear, input.TexC);

		if(alphaClip) // If alpha clipping is true
			clip(texColor.a - 0.1f);
	}

	return texColor;
};

float4 PSScene_Lights(VSOut input,
			   uniform bool alphaClip,
			   uniform bool useTex,
			   uniform int dirLightAmount,
			   uniform int pointLightAmount,
			   uniform int spotLightAmount) : SV_Target
{
	input.Normal = normalize(input.Normal);
	float4 texColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
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

DepthStencilState NoDepthWrites
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
};

technique11 Solid
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(false, true)));
		
		SetRasterizerState(Solidframe);
	}
};

technique11 SolidSkin
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, SkinVSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(false, true)));
		
		SetRasterizerState(Solidframe);
	}
};

technique11 SolidAlpha
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(true, true)));
		
		SetRasterizerState(Solidframe);
	}
};

technique11 SolidAlphaSkin
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, SkinVSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(true, true)));
		
		SetRasterizerState(Solidframe);
	}
}

technique11 Wire
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(false, false)));
		
		SetRasterizerState(Wireframe);
	}
};

technique11 AllLights
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene_Lights(false, true, 1, 0, 0)));
		
		SetRasterizerState(Solidframe);
	}
};

technique11 AllLightsAlpha
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene_Lights(true, true, 0, 2, 0)));
		
		SetRasterizerState(Solidframe);
	}
};

technique11 ShadowAlpha
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VSScene_Shadow()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSScene_Shadow(true, true, 0, 2, 0)));

		SetRasterizerState(Solidframe);
	}
};