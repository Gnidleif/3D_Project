//-------------------------------------------------------------------------
// Effect for calculating lights
//-------------------------------------------------------------------------

// Suffixes: L (Local), W (World), V (View), H (Homogeneous)

// Keep mirrored with C++ implementation (LightDef.h)

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Direction;
	float Padding;
};

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Attenuation;
	float Padding;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Attenuation;
	float Padding;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Reflect;
};

//=============================================================================
// Directional light
//=============================================================================
void ComputeDirectionalLight(
	Material mat,
	DirectionalLight light,
	float3 normal,
	float3 toEye,

	out float4 ambient,
	out float4 diffuse,
	out float4 specular)
{
	// Initialize outputs
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Light vector aims opposite the direction the light rays travel
	float3 lightVec = -light.Direction;

	ambient = mat.Ambient * light.Ambient;

	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Diffuse;
		specular = specFactor * mat.Specular * light.Specular;
	}
}

//=============================================================================
// Point light
//=============================================================================
void ComputePointLight(
	Material mat,		// Material
	PointLight light,	// Point light source
	float3 pos,			// Surface position
	float3 normal,		// Surface normal
	float3 toEye,		// Surface point being lit to the eye

	out float4 ambient,
	out float4 diffuse,
	out float4 specular)
{
	// Initialize outputs
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Define light vector (Direction from surface point to point light source)
	float3 lightVec = light.Position - pos;

	// Calculate distance from surface to light
	float dist = length(lightVec);

	// Test if out of range
	if (dist > light.Range)
		return;

	// Normalize light vector
	lightVec /= dist;

	// Ambient
	ambient = mat.Ambient * light.Ambient;

	// Begin calculating diffuse and specular
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if(diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Diffuse;
		specular = specFactor * mat.Specular * light.Specular;
	}

	// Attenuate
	float atten = 1.0f / dot(light.Attenuation, float3(1.0f, dist, dist*dist));

	diffuse *= atten;
	specular *= atten;
}

//=============================================================================
// Spot light
//=============================================================================
void ComputeSpotLight(
	Material mat,
	SpotLight light,
	float3 pos,
	float3 normal,
    float3 toEye,
	
	out float4 ambient,
	out float4 diffuse,
	out float4 specular)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Vector from surface to light
	float3 lightVec = light.Position - pos;

	// Distance from surface to light
	float dist = length(lightVec);

	// If distance is further than lights range, return
	if (dist > light.Range)
		return;

	// Normalize light vector
	lightVec /= dist;

	// Ambient
	ambient = mat.Ambient * light.Ambient;

	// Add diffuse and specular
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Diffuse;
		specular = specFactor * mat.Specular * light.Specular;
	}

	// Scale by spotlight factor
	float spot = pow(max(dot(-lightVec, light.Direction), 0.0f), light.Spot);

	// Scale by attenuation factor
	float attenuation = spot / dot(light.Attenuation, float3(1.0f, dist, dist*dist));

	ambient *= spot;
	diffuse *= attenuation;
	specular *= attenuation;
}

//====================================================================================
// Function which determines if a pixel is in a shadow.
// Essentially the factor is a scalar in the range 0 to 1.
// Though, with PCF, the factor is between 0 and 1 (the point is partially in shadow).
//====================================================================================

static const float SHADOW_EPSILON = 0.00001f;

float CalcShadowFactor(SamplerState samState, Texture2D shadowMap, float4 ProjTex, float screenX, float screenY)
{
	ProjTex.xy /= ProjTex.w;

	float2 smTex = float2(0.5f * ProjTex.x, -0.5f * ProjTex.y) + 0.5f;
	float depth = ProjTex.z / ProjTex.w;

	float dx = 1.0f / screenX;
	float dy = 1.0f / screenY;

	float2 texels[] =
	{
		float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(0.0f, dy), float2(dx, dy)
	};

	float4 s = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
	for(int i = 0; i < 4; ++i)
	{
		s[i] = (shadowMap.Sample(samState, smTex + texels[i]).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	}

	float2 texelPos = float2(smTex.x * screenX, smTex.y * screenY); //Transform to texel space

	float2 lerps = frac(texelPos);

	return lerp(lerp(s[0], s[1], lerps.x), lerp(s[2], s[3], lerps.x), lerps.y);
}

//===================================================================================
// Transforms a normal sample to world space
//===================================================================================
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float4 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1]
	float3 normalT = 2.0f * normalMapSample - 1.0f;

	// Build orthonormal basis
	float3 N = unitNormalW;
	float3 T = normalize(tangentW.xyz - dot(tangentW.xyz, N) * N);
	float3 B = tangentW.w * cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space
	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}