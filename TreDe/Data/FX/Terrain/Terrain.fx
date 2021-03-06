#include "../ShadowMap/ShadowMap.fx"

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

cbuffer cbFixed
{
	float texScale = 15.0f;
	float lightAddScale = 0.3f;
};

Texture2D gBlendMap;
Texture2D gTex0;
Texture2D gTex1;
Texture2D gTex2;
Texture2D gTex3;
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

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};

// Input Vertex shader
struct VSIn
{
	float3 PosL : POSITION; // Local position
	float3 Normal : NORMAL;
	float2 TexC : TEXCOORD;
};

// Input Pixel shader
struct VSOut
{
	float4 PosH : SV_Position; // Homogenous position
	float3 PosW : POSITION; // World position
	float3 Normal : NORMAL;
	float2 Tiled : TEXCOORD0;
	float2 Stretched : TEXCOORD1;
};

// VS -> PS
VSOut VSScene(VSIn input)
{
	VSOut output = (VSOut)0; // If something goes to shit, it'll be here
	float4x4 wvp = mul(gWorld, mul(gView, gProj));

	output.PosH = mul(float4(input.PosL, 1.0f), wvp);

	output.PosW = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	output.Normal = mul(input.Normal, (float3x3)gWorldInvTranspose);

	output.Tiled = texScale * input.TexC;
	output.Stretched = input.TexC;

	return output;
};

// Shadow map stuff
struct VSOut_Shadow
{
	float4 PosH : SV_Position;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 Tiled : TEXCOORD0;
	float2 Stretched : TEXCOORD1;
	float4 ProjTex : TEXCOORD2;
};

VSOut_Shadow VSScene_Shadow(VSIn input)
{
	VSOut_Shadow output = (VSOut_Shadow)0;
	float4x4 wvp = mul(gWorld, mul(gView, gProj));
	float4x4 lightwvp = mul(gWorld, gLightVP);

	output.PosH = mul(float4(input.PosL, 1.0f), wvp);

	output.PosW = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	output.Normal = mul(input.Normal, (float3x3)gWorldInvTranspose);

	output.Tiled = texScale * input.TexC;
	output.Stretched = input.TexC;

	output.ProjTex = mul(float4(input.PosL, 1.0f), lightwvp);

	return output;
};

float4 PSScene_Shadow(VSOut_Shadow input,
			   uniform bool useTex,
			   uniform int dirLightAmount,
			   uniform int pointLightAmount,
			   uniform int spotLightAmount) : SV_Target
{
	input.Normal = normalize(input.Normal);

	//samLinear
	float4 texColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
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
		float shadow = CalcShadowFactor(samClampLinear, gShadowMap, input.ProjTex, gScreenX, gScreenY);

		litColor = (texColor * (ambient + diffuse) + specular) * shadow;
		litColor += texColor * lightAddScale;
	}

	litColor.a = gMaterial.Diffuse.a * texColor.a;

	return litColor;
}

//

float4 PSScene(VSOut input,
			   uniform bool useTex) : SV_Target
{
	input.Normal = normalize(input.Normal);
	//samLinear
	float4 texColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
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

float4 PSScene_Lights(VSOut input,
			   uniform bool useTex,
			   uniform int dirLightAmount,
			   uniform int pointLightAmount,
			   uniform int spotLightAmount) : SV_Target
{
	input.Normal = normalize(input.Normal);

	//samLinear
	float4 texColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
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
			ComputeDirectionalLight(gMaterial, gDirLights[i], input.Normal, -toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		[unroll]
		for(int j = 0; j < pointLightAmount; ++j)
		{
			float4 A, D, S;
			ComputePointLight(gMaterial, gPointLights[j], input.PosW, input.Normal, -toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		[unroll]
		for(int k = 0; k < spotLightAmount; ++k)
		{
			float4 A, D, S;
			ComputeSpotLight(gMaterial, gSpotLights[k], input.PosW, input.Normal, -toEye, A, D, S);

			ambient += A;
			diffuse += D;
			specular += S;
		}

		litColor = texColor * (ambient + diffuse) + specular;
		litColor += texColor * lightAddScale;
	}

	// Materials later
	litColor.a = gMaterial.Diffuse.a * texColor.a;

	return litColor;
}

RasterizerState Wireframe
{
	FillMode = WireFrame;
	CullMode = Back;
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
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(true)));

		SetRasterizerState(Solidframe);
	}
};

technique11 Wire
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene(false)));

		SetRasterizerState(Wireframe);
	}
};

technique11 AllLights
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene_Lights(true, 0, 2, 0)));

		SetRasterizerState(Solidframe);
	}
};

technique11 Shadow
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, VSScene_Shadow()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_5_0, PSScene_Shadow(true, 0, 2, 0)));

		SetRasterizerState(Solidframe);
	}
};