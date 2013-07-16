#ifndef ANIMEVALUATOR_H
#define ANIMEVALUATOR_H
#include "d3dUtilities.h"
#include <map>
#include <assimp/scene.h>
#include "SkinDef.h"
#include <tuple>

class AnimEvaluator
{
public:
	AnimEvaluator();
	AnimEvaluator(const aiAnimation* animation);
	~AnimEvaluator();
	void SwitchPlayForward();
	void Evaluate(float time, std::map<std::string, SkinDef::Bone*>& bones);

public: // Get functions
	std::string GetName() const { return this->mName; }
//
//	UINT GetAnimIndexer() const { return this->mAnimIndexer; }
//
//	float GetLastTime() const { return this->mLastTime; }
//	float GetTicksPerSec() const { return this->mTicksPerSec; }
	//float GetDuration() const { return this->mDuration; }
//
//	bool GetPlayForward() const { return this->mPlayForward; }
//
//	std::vector<SkinDef::AnimChannel> GetChannels() const { return this->mChannels; }
//	std::vector<std::tuple<UINT, UINT, UINT>> GetLastPositions() const { return this->mLastPositions; }
//	std::vector<std::vector<XMFLOAT4X4>> GetTransforms() const { return this->mTransforms; }

	std::vector<XMFLOAT4X4>& GetTransformation(float dt) 
	{ return mTransforms[CalcFrameIndexAt(dt)]; }

	std::vector<XMFLOAT4X4>& GetTransformation(float dt, UINT frameStart, UINT frameEnd) 
	{ return mTransforms[CalcFrameIndexAt(dt, frameStart, frameEnd)]; }

	std::vector<XMFLOAT4X4>& GetTransformation(float dt, UINT frameStart, UINT frameEnd, bool playForward) 
	{ return mTransforms[CalcFrameIndexAt(dt, frameStart, frameEnd, playForward)]; }


//public: // Set functions
	//void InsertTransformation(XMFLOAT4X4 transformation) { this->mTransforms.push_back(transformation); }


//	void SetName(std::string name) { this->mName = name; }
//
//	void SetAnimIndexer(UINT animIndexer) { this->mAnimIndexer = animIndexer; }
//
//	void SetLastTime(float lastTime) { this->mLastTime = lastTime; }
//	void SetTicksPerSec(float ticksPerSec) { this->mTicksPerSec = ticksPerSec; }
//	void SetDuration(float duration) { this->mDuration = duration; }
//
//	void SetPlayForward(bool forwardOrNot) { this->mPlayForward = forwardOrNot; }
//
//	void SetChannels(std::vector<SkinDef::AnimChannel> channels) { this->mChannels = channels; }
//	void SetLastPositions(std::vector<std::tuple<UINT, UINT, UINT>> lastPositions) { this->mLastPositions = lastPositions; }
//	void SetTransforms(std::vector<std::vector<XMFLOAT4X4>> transforms) { this->mTransforms = transforms; }
//
//	void SetChannelsSize(UINT size) { this->mChannels.resize(size); }
//	void SetLastPositionsSize(UINT size) { this->mLastPositions.resize(size); }
//	void SetTransformsSize(UINT size) { this->mTransforms.resize(size); }

private: // Private functions
	UINT CalcFrameIndexAt(float time);
	UINT CalcFrameIndexAt(float time, UINT frameStart, UINT frameEnd);
	UINT CalcFrameIndexAt(float time, UINT frameStart, UINT frameEnd, bool playForward);

private:
	std::string mName;
	UINT mAnimIndexer;
	float mLastTime, mTicksPerSec, mDuration;
	bool mPlayForward;
	std::vector<SkinDef::AnimChannel> mChannels;
	std::vector<std::tuple<UINT, UINT, UINT>> mLastPositions;
	std::vector<std::vector<XMFLOAT4X4>> mTransforms;
};
#endif
