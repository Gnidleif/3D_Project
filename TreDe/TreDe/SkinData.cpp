#include "SkinData.h"

SkinData::SkinData()
	:
	mSkeleton(nullptr), mCurrAnimIndex(0)
{
}

SkinData::~SkinData(void)
{
	for(auto& it(mBoneNames.begin()); it != mBoneNames.end(); ++it)
		SafeDelete(it->second);
	for(auto& it(mBones.begin()); it != mBones.end(); ++it)
		SafeDelete(*it);
	mBoneIndices.clear();
	mAnimNameID.clear();
	mTransforms.clear();
	mAnimations.clear();
	SafeDelete(mSkeleton);
}

bool SkinData::SetAnimation(std::string name)
{
	auto& it = mAnimNameID.find(name);
	UINT oldIndex = mCurrAnimIndex;
	
	if(it != mAnimNameID.end())
		mCurrAnimIndex = it->second;

	return oldIndex != mCurrAnimIndex;
}

bool SkinData::SetAnimIndex(UINT animIndex)
{
	if((size_t)animIndex >= mAnimations.size())
		return false;

	UINT oldIndex = mCurrAnimIndex;
	mCurrAnimIndex = animIndex;
	return oldIndex != mCurrAnimIndex;
}

void SkinData::CalcTransform(float time)
{
	if((mCurrAnimIndex < 0) || (mCurrAnimIndex >= mAnimations.size()))
		return;

	mAnimations[mCurrAnimIndex].Evaluate(time, mBoneNames);
	this->UpdateTransforms(mSkeleton);
}

void SkinData::CalcBoneMatrices()
{
	for(UINT i(0); i != mTransforms.size(); ++i)
	{
		XMMATRIX offSet = XMLoadFloat4x4(&mBones[i]->mOffset);
		XMMATRIX globalTrans = XMLoadFloat4x4(&mBones[i]->mGlobalTrans);
		XMStoreFloat4x4(&mTransforms[i], XMMatrixMultiply(offSet, globalTrans));
	}
}

UINT SkinData::GetAnimIndex(std::string name)
{
	auto& it = mAnimNameID.find(name);
	UINT animIndex = 0;

	if(it != mAnimNameID.end())
		animIndex = it->second;
	return animIndex;
}

void SkinData::UpdateTransforms(SkinDef::Bone* node)
{
	SkinDef::CalculateBoneToWorld(node);
	for(auto& it(node->mChildren.begin()); it != node->mChildren.end(); ++it)
	{
		this->UpdateTransforms(*it);
	}
}