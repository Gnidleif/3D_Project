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
	this->mSkyTech = mFX->GetTechniqueByName("SkyTech");

	this->mWVP = mFX->GetVariableByName("gWVP")->AsMatrix();
	this->mCubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

// TerrainEffect functions
TerrainEffect::TerrainEffect(ID3D11Device* device, string filename)
	: VirtualEffect(device, filename)
{
	this->mTerrainSolidTech = mFX->GetTechniqueByName("TerrainSolidTech");
	this->mTerrainWireTech = mFX->GetTechniqueByName("TerrainWireTech");

	this->mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	this->mView = mFX->GetVariableByName("gView")->AsMatrix();
	this->mProj = mFX->GetVariableByName("gProj")->AsMatrix();
	this->mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();

	this->mBlendMap = mFX->GetVariableByName("gBlendMap")->AsShaderResource();
	this->mTex0 = mFX->GetVariableByName("gTex0")->AsShaderResource();
	this->mTex1 = mFX->GetVariableByName("gTex1")->AsShaderResource();
	this->mTex2 = mFX->GetVariableByName("gTex2")->AsShaderResource();
	this->mTex3 = mFX->GetVariableByName("gTex3")->AsShaderResource();

	this->mMaterial = mFX->GetVariableByName("gMaterial");
}

// NormalEffect functions
NormalEffect::NormalEffect(ID3D11Device* device, string filename)
	: VirtualEffect(device, filename)
{
	// Techniques
	this->mNormalSolidTech = mFX->GetTechniqueByName("NormalMapSolidTech");
	this->mNormalWireTech = mFX->GetTechniqueByName("NormalMapWireTech");

	// Variables
	this->mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	this->mView = mFX->GetVariableByName("gView")->AsMatrix();
	this->mProj = mFX->GetVariableByName("gProj")->AsMatrix();
	this->mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();

	this->mDiffuseMap = mFX->GetVariableByName("gDiffMap")->AsShaderResource();
	this->mNormalMap = mFX->GetVariableByName("gNormMap")->AsShaderResource();

	this->mMaterial = mFX->GetVariableByName("gMaterial");
}

// Effects functions
SkyEffect* Effects::SkyFX = nullptr;
TerrainEffect* Effects::TerrainFX = nullptr;
NormalEffect* Effects::NormalFX = nullptr;

void Effects::Initialize(ID3D11Device* device)
{
	SkyFX = new SkyEffect(device, "../Data/FX/Sky/Sky.fxo");
	TerrainFX = new TerrainEffect(device, "../Data/FX/Terrain/Terrain.fxo");
	NormalFX = new NormalEffect(device, "../Data/FX/NormalMap/NormalMap.fxo");
}

void Effects::Shutdown()
{
	SafeDelete(SkyFX);
	SafeDelete(TerrainFX);
	SafeDelete(NormalFX);
}