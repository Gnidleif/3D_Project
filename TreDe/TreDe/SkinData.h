#pragma once
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

public:
	void SetSkeleton(SkinDef::Bone* skeleton) { this->mSkeleton = skeleton; }
	void SetBoneName(UINT index, std::string name) { this->mBones[index]->mName = name; }

	void InsertBoneName(std::string key, SkinDef::Bone* bone) { this->mBoneNames[key] = bone; }
	void InsertBoneIndice(std::string key, UINT index) { this->mBoneIndices[key] = index; }

	void InsertAnimation(AnimEvaluator item) { this->mAnimations.push_back(item); }
	void InsertAnimNameID(std::string key, UINT id) { this->mAnimNameID[key] = id; }

	void SetAnimTransVector(UINT index, std::vector<std::vector<XMFLOAT4X4>> item) { this->mAnimations[index].SetTransVectorVector(item); }
	void InsertAnimTransform(UINT index, std::vector<XMFLOAT4X4> item) { this->mAnimations[index].InsertTransformation(item); }

	void InsertBone(SkinDef::Bone* bone) { this->mBones.push_back(bone); }

	void SetTransVectorSize(UINT size) { this->mTransforms.resize(size); }
	void SetTransVector(std::vector<XMFLOAT4X4> transforms) { this->mTransforms = transforms;}

public:
	std::vector<AnimEvaluator> GetAnimations() const { return this->mAnimations; }
	AnimEvaluator GetAnimation(int index) const { return this->mAnimations[index]; }
	std::map<std::string, SkinDef::Bone*> GetBoneNames() const { return this->mBoneNames; }
	std::vector<SkinDef::Bone*> GetBones() const { return this->mBones; }
	SkinDef::Bone* GetBone(UINT index) const { return this->mBones[index]; }
	std::vector<XMFLOAT4X4> GetTransVector() const { return this->mTransforms; }

	std::vector<XMFLOAT4X4>& GetTransforms(float dt) 
	{ return mAnimations[mCurrAnimIndex].GetTransformation(dt); }

	std::vector<XMFLOAT4X4>& GetTransforms(float dt, UINT animIndex)
	{ return mAnimations[animIndex].GetTransformation(dt); }

	std::vector<XMFLOAT4X4>& GetTransforms(float dt, UINT animIndex, UINT start, UINT end)
	{ return mAnimations[animIndex].GetTransformation(dt, start, end); }

	std::vector<XMFLOAT4X4>& GetTransforms(float dt, UINT animIndex, UINT start, UINT end, bool playForward)
	{ return mAnimations[animIndex].GetTransformation(dt, start, end, playForward); }

private:
	void UpdateTransforms(SkinDef::Bone* &node);

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
