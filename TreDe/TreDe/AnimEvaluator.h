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
	float GetDuration() const { return this->mDuration; }
	float GetTicksPerSec() const { return this->mTicksPerSec; }
	std::vector<std::vector<XMFLOAT4X4>> GetTransVector() const { return this->mTransforms; }

	std::vector<XMFLOAT4X4>& GetTransformation(float dt) 
	{ return mTransforms[CalcFrameIndexAt(dt)]; }

	std::vector<XMFLOAT4X4>& GetTransformation(float dt, UINT frameStart, UINT frameEnd) 
	{ return mTransforms[CalcFrameIndexAt(dt, frameStart, frameEnd)]; }

	std::vector<XMFLOAT4X4>& GetTransformation(float dt, UINT frameStart, UINT frameEnd, bool playForward) 
	{ return mTransforms[CalcFrameIndexAt(dt, frameStart, frameEnd, playForward)]; }

public: // Set functions
	void InsertTransformation(std::vector<XMFLOAT4X4> transformation) { this->mTransforms.push_back(transformation); }

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
