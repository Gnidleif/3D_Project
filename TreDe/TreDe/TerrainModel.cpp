#include "TerrainModel.h"
using namespace std;

TerrainModel::TerrainModel(string filename)
	: VirtualModel(),
	mMaterial(new Material()),
	mMesh(new TerrainMesh())
{
	CreateMatsAndMeshes(filename);
	//SetBuffers(device);

	mBlendMap = Texture->GetTexture("../Data/Textures/BlendMap.dds");
	mTex0 = Texture->GetTexture("../Data/Textures/grass.dds");
	mTex1 = Texture->GetTexture("../Data/Textures/darkgrass.dds");
	mTex2 = Texture->GetTexture("../Data/Textures/stone.dds");
	mTex3 = Texture->GetTexture("../Data/Textures/lava.dds");

	Effects::TerrainFX->SetBlendMap(mBlendMap);
	Effects::TerrainFX->SetTex0(mTex0);
	Effects::TerrainFX->SetTex1(mTex1);
	Effects::TerrainFX->SetTex2(mTex2);
	Effects::TerrainFX->SetTex3(mTex3);
}

TerrainModel::~TerrainModel(void)
{
	SafeDelete(mMaterial);
	SafeDelete(mMesh);
}

void TerrainModel::CreateMatsAndMeshes(string filename)
{
	// Sätt in skit för material här sen
	Loader->CreateHeightMap(this->mMesh, filename, 15.0f, 1);
}