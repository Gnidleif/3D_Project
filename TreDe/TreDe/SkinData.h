#ifndef SKINDATA_H
#define SKINDATA_H
#include "SkinDef.h"
#include "AnimEvaluator.h"
#include "d3dUtilities.h"
#include <map>

class SkinData
{
public:
	SkinData();
	~SkinData();
	bool SetAnimation(std::string name);
	bool SetAnimIndex(UINT animIndex);
	void CalcTransform(float time);
	void CalcBoneMatrices();
	UINT GetAnimIndex(std::string name);

private:
	void UpdateTransforms(SkinDef::Bone* node);

private:
	SkinDef::Bone* mSkeleton;
	std::map<std::string, SkinDef::Bone*> mBoneNames;
	std::map<std::string, UINT> mBoneIndices;
	std::map<std::string, UINT> mAnimNameID;
	std::vector<SkinDef::Bone*> mBones;
	std::vector<XMFLOAT4X4> mTransforms;
	std::vector<AnimEvaluator> mAnimations;
	UINT mCurrAnimIndex;
};
#endif
