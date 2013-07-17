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
	SkinnedModel(std::string filename, std::string texPath);
	~SkinnedModel(void);
	void ApplyEffects();

public:
	std::vector<SkinnedMesh*> GetMeshes() const { return this->mMeshes; }
	SkinnedMesh* GetMesh(UINT index) const { return this->mMeshes[index]; }
	std::vector<Material> GetMaterials() const { return this->mMaterials; }
	std::vector<ID3D11ShaderResourceView*> GetDiffMap() const { return this->mDiffMapSRV; }
	std::vector<ID3D11ShaderResourceView*> GetNormalMap() const { return this->mNormalMapSRV; }

	SkinData* GetSkinData() const { return this->mSkinData; }

private:
	void CreateMatsAndMeshes(std::string filename);

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