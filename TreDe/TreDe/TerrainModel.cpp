#include "TerrainModel.h"
using namespace std;

TerrainModel::TerrainModel(ID3D11Device* device, string filename)
	: VirtualModel(device),
	mMesh(new TerrainMesh())
{
	this->CreateMatsAndMeshes(filename);
	// Hardcoded, I know, but it's just better this way right now so fuck off
	mBlendMap = Texture->GetTexture("../Data/Textures/BlendMap.dds");
	mTex0 = Texture->GetTexture("../Data/Textures/grass.dds");
	mTex1 = Texture->GetTexture("../Data/Textures/darkgrass.dds");
	mTex2 = Texture->GetTexture("../Data/Textures/stone.dds");
	mTex3 = Texture->GetTexture("../Data/Textures/lava.dds");
	this->CreateBuffers();
}

TerrainModel::~TerrainModel(void)
{
	SafeDelete(mMesh);
}

void TerrainModel::ApplyEffects()
{
	Effects::TerrainFX->SetBlendMap(mBlendMap);
	Effects::TerrainFX->SetTex0(mTex0);
	Effects::TerrainFX->SetTex1(mTex1);
	Effects::TerrainFX->SetTex2(mTex2);
	Effects::TerrainFX->SetTex3(mTex3);
	Effects::TerrainFX->SetMaterial(this->mMaterial);
}

void TerrainModel::CreateMatsAndMeshes(string filename)
{
	// S�tt in skit f�r material h�r sen
	Loader->CreateHeightMap(this->mMesh, filename, 15.0f, 1);

	// Some more hardcoded bullshit!
	this->mMaterial.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	this->mMaterial.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f);
	this->mMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	this->mMaterial.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

void TerrainModel::CreateBuffers()
{
	this->mMesh->SetVertexBuffer(this->mDevice, &mMesh->GetVertices()[0], mMesh->GetVertices().size());
	this->mMesh->SetIndexBuffer(this->mDevice, &mMesh->GetIndices()[0], mMesh->GetIndices().size());
}