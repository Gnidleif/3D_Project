//-----------------------------------------------------------------------
// Effect which shades a sky
//-----------------------------------------------------------------------

cbuffer cbPerFrame
{
	float4x4 gWVP;
};

// Cube map
TextureCube gCubeMap;

SamplerState samTriLinearSam
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
float4 PS(VertexOut pin) : SV_Target
{
	float4 Color = gCubeMap.Sample(samTriLinearSam, pin.PosL);
	return Color;
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

RasterizerState Solidframe
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

technique11 SkyTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
        
        SetRasterizerState(NoCulling);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}

technique11 WireSkyTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));

		SetRasterizerState(Wireframe);
		SetDepthStencilState(LessEqualDSS, 0);
	}
}