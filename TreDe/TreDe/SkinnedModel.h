#ifndef SKINNEDMODEL_H
#define SKINNEDMODEL_H
#include "VirtualModel.h"
#include "Meshes.h"
#include "LightDef.h"
#include "LoaderClass.h"
#include "SkinData.h"

class SkinnedModel : public VirtualModel
{
public:
	SkinnedModel(ID3D11Device* device, std::string filename, std::string texPath);
	~SkinnedModel(void);

public:
	SkinnedMesh* GetMesh(UINT index) const { return this->mMeshes[index]; }
	Material GetMaterial(UINT index) const { return this->mMaterials[index]; }
	ID3D11ShaderResourceView* GetDiffMapSRV(UINT index) const { return this->mDiffMapSRV[index]; }
	ID3D11ShaderResourceView* GetNormalMapSRV(UINT index) const { return this->mNormalMapSRV[index]; }

	SkinData* GetSkinData() const { return this->mSkinData; }

private:
	void CreateMatsAndMeshes(std::string filename);
	void CreateBuffers();

private:
	std::vector<Material> mMaterials;
	std::vector<SkinnedMesh*> mMeshes;
	std::vector<ID3D11ShaderResourceView*> mDiffMapSRV;
	std::vector<ID3D11ShaderResourceView*> mNormalMapSRV;
	std::vector<GenericMaterial> mGenMats;

	SkinData* mSkinData;

public:
	struct Instance
	{
		SkinnedModel* mModel;
		XMFLOAT4X4 mWorld;

		std::vector<XMFLOAT4X4> mFinalTransforms;
		std::string mAnimName;
		UINT mAnimIndex;
		float mTimePos;
		UINT mStart, mEnd;
		bool mPlayForward;

		void Update(float dt);
	};
};
#endif