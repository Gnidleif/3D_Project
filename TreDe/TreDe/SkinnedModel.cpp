#include "SkinnedModel.h"
#include "Effects.h"
using namespace std;

SkinnedModel::SkinnedModel(string filename, string texPath)
	: VirtualModel(),
	mMeshes(vector<SkinnedMesh*>()),
	mMaterials(vector<Material>()),
	mDiffMapSRV(vector<ID3D11ShaderResourceView*>()),
	mNormalMapSRV(vector<ID3D11ShaderResourceView*>()),
	mGenMats(vector<GenericMaterial>()),
	mSkinData(SkinData())
{
	this->CreateMatsAndMeshes(filename);

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

	for(UINT i(0); i != this->mMeshCount; ++i)
	{
		Effects::NormalFX->SetDiffuseMap(this->mDiffMapSRV[mMeshes[i]->GetMaterialIndex()]);
		Effects::NormalFX->SetNormalMap(this->mNormalMapSRV[mMeshes[i]->GetMaterialIndex()]);
		for(UINT j(0); j != this->mMaterialCount; ++j)
		{
			Effects::NormalFX->SetMaterial(&this->mMaterials[mMeshes[i]->GetMaterialIndex()]);
		}
	}
}

SkinnedModel::~SkinnedModel(void)
{
	for(auto& it(mMeshes.begin()); it != mMeshes.end(); ++it)
		SafeDelete(*it);
	mMeshes.clear();
	mMaterials.clear();
	mDiffMapSRV.clear();
	mNormalMapSRV.clear();
	mGenMats.clear();
}	

void SkinnedModel::CreateMatsAndMeshes(string filename)
{
	// Loader->CreateSkinnedObject(mGenMats, mMeshes, filename, mSkinData);
	this->mMeshCount = mMeshes.size();
	this->mMaterialCount = mGenMats.size();
}

void SkinnedModel::Instance::Update(float dt)
{
	this->Instance::mTimePos += dt;
	this->Instance::mFinalTransforms = this->Instance::mModel->mSkinData.GetTransforms(
		this->Instance::mTimePos,
		this->Instance::mAnimIndex,
		this->Instance::mStart,
		this->Instance::mEnd,
		this->Instance::mPlayForward);
}