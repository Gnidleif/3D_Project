#include "Effects.h"
#include <iostream>

using namespace std;
// Effects functions
VirtualEffect::VirtualEffect(ID3D11Device* device, std::string filename)
	:
	mFX(nullptr)
{
	int size = 0;
	ifstream in(filename, ios::binary);
	if(in.is_open())
	{
		in.seekg(0, ios_base::end);
		size = (int)in.tellg();
		in.seekg(0, ios_base::beg);
		std::vector<char> compiledShader(size);

		in.read(&compiledShader[0], size);
		D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &mFX);
	}
	else
		cout << "Couldn't open " + filename << endl;
	in.close();
}


VirtualEffect::~VirtualEffect(void)
{
	SafeRelease(mFX);
}

// SkyEffect functions
SkyEffect::SkyEffect(ID3D11Device* device, string filename)
	: VirtualEffect(device, filename)
{
	this->mSolid = mFX->GetTechniqueByName("Solid");
	this->mWire = mFX->GetTechniqueByName("Wire");

	this->mWVP = mFX->GetVariableByName("gWVP")->AsMatrix();
	this->mCubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

// TerrainEffect functions
TerrainEffect::TerrainEffect(ID3D11Device* device, string filename)
	: VirtualEffect(device, filename)
{
	this->mSolid = mFX->GetTechniqueByName("Solid");
	this->mWire = mFX->GetTechniqueByName("Wire");
	this->mAllLights = mFX->GetTechniqueByName("AllLights");
	this->mShadow = mFX->GetTechniqueByName("Shadow");

	this->mEyePos = mFX->GetVariableByName("gEyePos")->AsVector();

	this->mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	this->mView = mFX->GetVariableByName("gView")->AsMatrix();
	this->mProj = mFX->GetVariableByName("gProj")->AsMatrix();
	this->mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	this->mLightVP = mFX->GetVariableByName("gLightVP")->AsMatrix();

	this->mBlendMap = mFX->GetVariableByName("gBlendMap")->AsShaderResource();
	this->mTex0 = mFX->GetVariableByName("gTex0")->AsShaderResource();
	this->mTex1 = mFX->GetVariableByName("gTex1")->AsShaderResource();
	this->mTex2 = mFX->GetVariableByName("gTex2")->AsShaderResource();
	this->mTex3 = mFX->GetVariableByName("gTex3")->AsShaderResource();
	this->mShadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	this->mDirLights = mFX->GetVariableByName("gDirLights");
	this->mPointLights = mFX->GetVariableByName("gPointLights");
	this->mSpotLights = mFX->GetVariableByName("gSpotLights");

	this->mMaterial = mFX->GetVariableByName("gMaterial");
}

// NormalEffect functions
NormalEffect::NormalEffect(ID3D11Device* device, string filename)
	: VirtualEffect(device, filename)
{
	// Techniques
	this->mSolid = mFX->GetTechniqueByName("Solid");
	this->mSolidSkin = mFX->GetTechniqueByName("SolidSkin");
	this->mSolidAlpha = mFX->GetTechniqueByName("SolidAlpha");
	this->mSolidAlphaSkin = mFX->GetTechniqueByName("SolidAlphaSkin");
	this->mWire = mFX->GetTechniqueByName("Wire");
	this->mAllLights = mFX->GetTechniqueByName("AllLights");
	this->mAllLightsAlpha = mFX->GetTechniqueByName("AllLightsAlpha");
	this->mShadowAlpha = mFX->GetTechniqueByName("ShadowAlpha");

	this->mEyePos = mFX->GetVariableByName("gEyePos")->AsVector();

	// Variables
	this->mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	this->mView = mFX->GetVariableByName("gView")->AsMatrix();
	this->mProj = mFX->GetVariableByName("gProj")->AsMatrix();
	this->mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	this->mLightVP = mFX->GetVariableByName("gLightVP")->AsMatrix();

	this->mBoneTransforms = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();

	this->mDiffuseMap = mFX->GetVariableByName("gDiffMap")->AsShaderResource();
	this->mNormalMap = mFX->GetVariableByName("gNormMap")->AsShaderResource();
	this->mShadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	this->mDirLights = mFX->GetVariableByName("gDirLights");
	this->mPointLights = mFX->GetVariableByName("gPointLights");
	this->mSpotLights = mFX->GetVariableByName("gSpotLights");

	this->mMaterial = mFX->GetVariableByName("gMaterial");
}

// TessellationEffect functions
TessellationEffect::TessellationEffect(ID3D11Device* device, string filename)
	: VirtualEffect(device, filename)
{
	// Techniques
	this->mSolid = mFX->GetTechniqueByName("Solid");
	this->mSolidAlpha = mFX->GetTechniqueByName("SolidAlpha");
	this->mWire = mFX->GetTechniqueByName("Wire");
	this->mAllLights = mFX->GetTechniqueByName("AllLights");
	this->mAllLightsAlpha = mFX->GetTechniqueByName("AllLightsAlpha");

	this->mEyePos = mFX->GetVariableByName("gEyePos")->AsVector();

	// Variables
	this->mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	this->mView = mFX->GetVariableByName("gView")->AsMatrix();
	this->mProj = mFX->GetVariableByName("gProj")->AsMatrix();
	this->mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();

	this->mDiffuseMap = mFX->GetVariableByName("gDiffMap")->AsShaderResource();
	this->mNormalMap = mFX->GetVariableByName("gNormMap")->AsShaderResource();

	this->mDirLights = mFX->GetVariableByName("gDirLights");
	this->mPointLights = mFX->GetVariableByName("gPointLights");
	this->mSpotLights = mFX->GetVariableByName("gSpotLights");

	this->mMaterial = mFX->GetVariableByName("gMaterial");

	this->mMinDist = mFX->GetVariableByName("gMinDist")->AsScalar();
	this->mMaxDist = mFX->GetVariableByName("gMaxDist")->AsScalar();
	this->mMinTess = mFX->GetVariableByName("gMinTess")->AsScalar();
	this->mMaxTess = mFX->GetVariableByName("gMaxTess")->AsScalar();
}

// TessellationEffect functions
TerrainTessellationEffect::TerrainTessellationEffect(ID3D11Device* device, string filename)
	: VirtualEffect(device, filename)
{
	// Techniques
	this->mSolid = mFX->GetTechniqueByName("Solid");
	this->mWire = mFX->GetTechniqueByName("Wire");
	this->mAllLights = mFX->GetTechniqueByName("AllLights");

	this->mEyePos = mFX->GetVariableByName("gEyePos")->AsVector();

	// Variables
	this->mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	this->mView = mFX->GetVariableByName("gView")->AsMatrix();
	this->mProj = mFX->GetVariableByName("gProj")->AsMatrix();
	this->mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();

	this->mBlendMap = mFX->GetVariableByName("gBlendMap")->AsShaderResource();
	this->mTex0 = mFX->GetVariableByName("gTex0")->AsShaderResource();
	this->mTex1 = mFX->GetVariableByName("gTex1")->AsShaderResource();
	this->mTex2 = mFX->GetVariableByName("gTex2")->AsShaderResource();
	this->mTex3 = mFX->GetVariableByName("gTex3")->AsShaderResource();

	this->mDirLights = mFX->GetVariableByName("gDirLights");
	this->mPointLights = mFX->GetVariableByName("gPointLights");
	this->mSpotLights = mFX->GetVariableByName("gSpotLights");

	this->mMaterial = mFX->GetVariableByName("gMaterial");

	this->mMinDist = mFX->GetVariableByName("gMinDist")->AsScalar();
	this->mMaxDist = mFX->GetVariableByName("gMaxDist")->AsScalar();
	this->mMinTess = mFX->GetVariableByName("gMinTess")->AsScalar();
	this->mMaxTess = mFX->GetVariableByName("gMaxTess")->AsScalar();
}

// Shadow map effect constructor
ShadowMapEffect::ShadowMapEffect(ID3D11Device* device, std::string filename)
	: VirtualEffect(device, filename)
{
	this->mShadowTech = mFX->GetTechniqueByName("ShadowTech");
	
	this->mWorld = mFX->GetVariableByName("sWorld")->AsMatrix();
	this->mLightVP = mFX->GetVariableByName("sLightVP")->AsMatrix();

	this->mScreenX = mFX->GetVariableByName("gScreenX")->AsScalar();
	this->mScreenY = mFX->GetVariableByName("gScreenY")->AsScalar();
}

// Particle effect constructor
ParticleEffect::ParticleEffect(ID3D11Device* device, std::string filename)
	: VirtualEffect(device, filename)
{
	this->mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	
	this->mEyePos = mFX->GetVariableByName("gEyePos")->AsVector();
	this->mEmitPos = mFX->GetVariableByName("gEmitPos")->AsVector();
	this->mEmitDir = mFX->GetVariableByName("gEmitDir")->AsVector();

	this->mGameTime = mFX->GetVariableByName("gGameTime")->AsScalar();
	this->mTimeStep = mFX->GetVariableByName("gTimeStep")->AsScalar();

	this->mTexArray = mFX->GetVariableByName("gTexArray")->AsShaderResource();
	this->mRandomTex = mFX->GetVariableByName("gRandomTex")->AsShaderResource();
}

SunEffect::SunEffect(ID3D11Device* device, std::string filename)
	: ParticleEffect(device, filename)
{
	this->mDraw = mFX->GetTechniqueByName("Draw");
}

// Effects functions
SkyEffect* Effects::SkyFX = nullptr;
TerrainEffect* Effects::TerrainFX = nullptr;
NormalEffect* Effects::NormalFX = nullptr;
TessellationEffect* Effects::TessFX = nullptr;
TerrainTessellationEffect* Effects::TerrTessFX = nullptr;
ShadowMapEffect* Effects::ShadowFX = nullptr;
SunEffect* Effects::SunFX = nullptr;

void Effects::Initialize(ID3D11Device* device)
{
	SkyFX = new SkyEffect(device, "../Data/FX/Sky/Sky.fxo");
	TerrainFX = new TerrainEffect(device, "../Data/FX/Terrain/Terrain.fxo");
	NormalFX = new NormalEffect(device, "../Data/FX/NormalMap/NormalMap.fxo");
	TessFX = new TessellationEffect(device, "../Data/FX/Tessellation/Tessellation.fxo");
	TerrTessFX = new TerrainTessellationEffect(device, "../Data/FX/TerrainTessellation/TerrainTessellation.fxo");
	ShadowFX = new ShadowMapEffect(device, "../Data/FX/ShadowMap/ShadowMap.fxo");
	SunFX = new SunEffect(device, "../Data/FX/Particle/Sun.fxo");
}

void Effects::Shutdown()
{
	SafeDelete(SkyFX);
	SafeDelete(TerrainFX);
	SafeDelete(NormalFX);
	SafeDelete(TessFX);
	SafeDelete(TerrTessFX);
	SafeDelete(ShadowFX);
	SafeDelete(SunFX);
}