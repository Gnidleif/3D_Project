//-----------------------------------------------------------------------
// Effect which shades a sky
//-----------------------------------------------------------------------

cbuffer cbPerFrame
{
	float4x4 gWVP;
};

// Cube map
TextureCube gCubeMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VSIn
{
	float3 PosL : POSITION;
};

struct VSOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

VSOut VSScene(VSIn input)
{
	VSOut output = (VSOut)0;
	
	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	output.PosH = mul(float4(input.PosL, 1.0f), gWVP).xyww;
	
	// Use local vertex position as cubemap lookup vector.
	output.PosL = input.PosL;
	
	return output;
}

float4 PSScene(VSOut input,
		  uniform bool useTex) : SV_Target
{
	float4 texColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	if(useTex)
		texColor = gCubeMap.Sample(samLinear, input.PosL);
	return texColor;
}

RasterizerState NoCulling
{
    CullMode = None;
};

DepthStencilState LessEqualDSS
{
    DepthFunc = LESS_EQUAL;
};

RasterizerState Wireframe
{
	FillMode = WireFrame;
	CullMode = None;
	FrontCounterClockwise = false;
};

technique11 Solid
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VSScene()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSScene(true)));
        
        SetRasterizerState(NoCulling);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}

technique11 Wire
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VSScene()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSScene(false)));

		SetRasterizerState(Wireframe);
		SetDepthStencilState(LessEqualDSS, 0);
	}
}