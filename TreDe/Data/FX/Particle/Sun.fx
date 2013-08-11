cbuffer cbPerFrame
{
	float4x4 gViewProj;

	float3 gEyePos;
	float3 gEmitPos;
	float3 gEmitDir;

	float gGameTime;
	float gTimeStep;
};

cbuffer cbFixed
{
	float3 gAccel = { 0.0f, 0.0f, 0.0f };
	float2 gQuadTexC[4] =
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};

Texture2DArray gTexArray;
Texture1D gRandomTex;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

float3 RandUnitVec3(float offset)
{
	float u = (gGameTime + offset);
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	return normalize(v);
}

// StreamOut

#define PT_EMITTER 0
#define PT_FLARE 1

struct Particle
{
	float3 InitPosW : POSITION;
	float3 InitVelW : VELOCITY;
	float2 SizeW : SIZE;
	float Age : AGE;
	uint Type : TYPE;
};

Particle StreamOutVSScene(Particle input)
{
	return input;
}

[maxvertexcount(2)]
void StreamOutGSScene(point Particle input[1], inout PointStream<Particle> stream)
{
	input[0].Age += gTimeStep;

	if(input[0].Type == PT_EMITTER)
	{
		if(input[0].Age > 0.005f)
		{
			float3 vRandom = RandUnitVec3(0.0f);
			vRandom.x *= 0.3f;
			vRandom.z *= 0.3f;
			vRandom.y *= 0.3f;

			Particle p = (Particle)0;

			p.InitPosW = gEmitPos.xyz;
			// Change these things to change the stuff that happens
			p.InitVelW = 10.0f * vRandom.xyz;
			p.SizeW = float2(100.0f, 100.0f);
			p.Age = 0.0f;
			p.Type = PT_FLARE;
			//
			stream.Append(p);
			input[0].Age = 0.0f;
		}

		stream.Append(input[0]);
	}
	else
	{
		if(input[0].Age <= 0.1f)
			stream.Append(input[0]);
	}
}

GeometryShader GS_StreamOut = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGSScene()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");

technique11 StreamOut
{

}

// Draw

struct VSOut
{
	float3 PosW : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint Type : TYPE;
};

VSOut DrawVS(Particle input)
{
	VSOut output = (VSOut)0;

	float time = input.Age;

	output.PosW = 0.5f * time * time * input.InitVelW + time * input.InitVelW + input.InitPosW;

	float opacity = 1.0f - smoothstep(0.0f, 1.0f, time/1.0f);
	output.Color = float4(1.0f, 1.0f, 0.5f, opacity);

	output.SizeW = input.SizeW;
	output.Type = input.Type;
	
	return output;
}

struct GSOut
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
	float2 TexC : TEXCOORD;
};

[maxvertexcount(4)]
void DrawGS(point VSOut input[1],
			inout TriangleStream<GSOut> stream)
{
	if(input[0].Type != PT_EMITTER)
	{
		float3 look  = normalize(gEyePos.xyz - input[0].PosW);
		float3 right = normalize(cross(float3(0,1,0), look));
		float3 up    = cross(look, right);

		float4x4 world;
		world[0] = float4(right,       0.0f);
		world[1] = float4(up,          0.0f);
		world[2] = float4(look,        0.0f);
		world[3] = float4(input[0].PosW, 1.0f);

		float4x4 wvp = mul(world, gViewProj);

		float halfWidth  = 0.5f*input[0].SizeW.x;
		float halfHeight = 0.5f*input[0].SizeW.y;
	
		float4 v[4];
		v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
		v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
		v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
		v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);

		GSOut output = (GSOut)0;
		[unroll]
		for(int i = 0; i < 4; ++i)
		{
			output.PosH  = mul(v[i], wvp);
			output.TexC  = gQuadTexC[i];
			output.Color = input[0].Color;
			stream.Append(output);
		}	
	}
}

float4 DrawPS(GSOut input) : SV_TARGET
{
	return gTexArray.Sample(samLinear, float3(input.TexC, 1.0f)) * input.Color;
}

technique11 Draw // One technique, two passes, hellz yeah!
{
	pass p0 // The streamout pass
	{
		SetVertexShader(CompileShader(vs_5_0, StreamOutVSScene()));
		SetGeometryShader(GS_StreamOut);
		SetPixelShader(NULL);

		SetDepthStencilState(DisableDepth, 0);
	}

	pass p1 // The draw pass
	{
		SetVertexShader(CompileShader(vs_5_0, DrawVS()));
		SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
		SetPixelShader(CompileShader(ps_5_0, DrawPS()));

		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState(DisableDepth, 0);
	}
}