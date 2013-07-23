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

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

//=======================================================================
// Vertex shader
//=======================================================================
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWVP).xyww;
	
	// Use local vertex position as cubemap lookup vector.
	vout.PosL = vin.PosL;
	
	return vout;
}

//=======================================================================
// Pixel shader
//=======================================================================
float4 PS(VertexOut pin,
		  uniform bool useTex) : SV_Target
{
	float4 texColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	if(useTex)
		texColor = gCubeMap.Sample(samLinear, pin.PosL);
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
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
        
        SetRasterizerState(NoCulling);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}

technique11 Wire
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(false)));

		SetRasterizerState(Wireframe);
		SetDepthStencilState(LessEqualDSS, 0);
	}
}