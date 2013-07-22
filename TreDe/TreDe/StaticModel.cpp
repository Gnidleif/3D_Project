#include "StaticModel.h"
#include "Effects.h"
using namespace std;

StaticModel::StaticModel(ID3D11Device* device, string filename, string texPath)
	: VirtualModel(device),
	mMeshes(vector<StaticMesh*>()),
	mMaterials(vector<Material>()),
	mDiffMapSRV(vector<ID3D11ShaderResourceView*>()),
	mNormalMapSRV(vector<ID3D11ShaderResourceView*>()),
	mGenMats(vector<GenericMaterial>())
{
	CreateMatsAndMeshes(filename);

	for(UINT i(0); i != mMaterialCount; ++i)
	{
		mMaterials.push_back(mGenMats[i].mMaterial);
		ID3D11ShaderResourceView* tempDiffMap = nullptr;
		if(mGenMats[i].mDiffMapName != "")
		{
			tempDiffMap = Texture->GetTexture(texPath + mGenMats[i].mDiffMapName);
		}
		mDiffMapSRV.push_back(tempDiffMap);

		ID3D11ShaderResourceView* tempNormalMap = nullptr;
		if(mGenMats[i].mNormalMapName != "")
		{
			tempNormalMap = Texture->GetTexture(texPath + mGenMats[i].mNormalMapName);
		}
		mNormalMapSRV.push_back(tempNormalMap);
	}
	this->CreateBuffers();
}

StaticModel::~StaticModel(void)
{
	for(auto& it(mMeshes.begin()); it != mMeshes.end(); ++it)
	{
		SafeDelete(*it);
	}
	mMeshes.clear();
	mMaterials.clear();
	mDiffMapSRV.clear();
	mNormalMapSRV.clear();
	mGenMats.clear();
}

void StaticModel::ApplyEffects()
{
	for(UINT i(0); i != this->mMeshCount; ++i)
	{
		Effects::NormalFX->SetDiffuseMap(this->mDiffMapSRV[mMeshes[i]->GetMaterialIndex()]);
		Effects::NormalFX->SetNormalMap(this->mNormalMapSRV[mMeshes[i]->GetMaterialIndex()]);
		Effects::NormalFX->SetMaterial(this->mMaterials[mMeshes[i]->GetMaterialIndex()]);
	}
}

void StaticModel::ApplyTessEffects()
{
	for(UINT i(0); i != this->mMeshCount; ++i)
	{
		Effects::TessFX->SetDiffuseMap(this->mDiffMapSRV[mMeshes[i]->GetMaterialIndex()]);
		Effects::TessFX->SetNormalMap(this->mNormalMapSRV[mMeshes[i]->GetMaterialIndex()]);
		Effects::TessFX->SetMaterial(this->mMaterials[mMeshes[i]->GetMaterialIndex()]);
	}
}

void StaticModel::CreateMatsAndMeshes(string filename)
{
	Loader->CreateStaticObject(mGenMats, mMeshes, filename);
	this->mMeshCount = mMeshes.size();
	this->mMaterialCount = mGenMats.size();
}

void StaticModel::CreateBuffers()
{
	for(UINT i(0); i != mMeshCount; ++i)
	{
		this->mMeshes[i]->SetVertexBuffer(this->mDevice, &mMeshes[i]->GetVertices()[0], mMeshes[i]->GetVertices().size());
		this->mMeshes[i]->SetIndexBuffer(this->mDevice, &mMeshes[i]->GetIndices()[0], mMeshes[i]->GetIndices().size());
	}
}