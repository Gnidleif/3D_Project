#include "AnimEvaluator.h"

AnimEvaluator::AnimEvaluator()
	: 
	mAnimIndexer(0),
	mLastTime(0.0f), mTicksPerSec(0.0f), mDuration(0.0f),
	mPlayForward(true),
	mChannels(std::vector<SkinDef::AnimChannel>()), 
	mLastPositions(std::vector<std::tuple<UINT, UINT, UINT>>()), 
	mTransforms(std::vector<std::vector<XMFLOAT4X4>>())
{
}

AnimEvaluator::AnimEvaluator(const aiAnimation* animation)
{
	this->mName = std::string(animation->mName.data, animation->mName.length);
	if(mName.size() == 0)
	{
		std::ostringstream ss;
		ss << "Animation" << mAnimIndexer;
		mName = ss.str();
	}

	this->mTicksPerSec = static_cast<float>(animation->mTicksPerSecond != 0.0f ? animation->mTicksPerSecond : 100.0f);
	this->mDuration = static_cast<float>(animation->mDuration);

	mChannels.resize(animation->mNumChannels);
	for(UINT i(0); i != mChannels.size(); ++i)
	{
		mChannels[i].mName = animation->mChannels[i]->mNodeName.data;

		for(UINT j(0); j != animation->mChannels[i]->mNumPositionKeys; ++j)
			mChannels[i].mPosKeys.push_back(animation->mChannels[i]->mPositionKeys[j]);

		for(UINT j(0); j != animation->mChannels[i]->mNumRotationKeys; ++j)
			mChannels[i].mRotKeys.push_back(animation->mChannels[i]->mRotationKeys[j]);

		for(UINT j(0); j != animation->mChannels[i]->mNumScalingKeys; ++j)
			mChannels[i].mScaleKeys.push_back(animation->mChannels[i]->mScalingKeys[j]);
	}

	mLastPositions.resize(animation->mNumChannels, std::make_tuple(0, 0, 0));
}

AnimEvaluator::~AnimEvaluator()
{
	mChannels.clear();
	mLastPositions.clear();
	mTransforms.clear();
}

void AnimEvaluator::SwitchPlayForward()
{
	if(mPlayForward)
		mPlayForward = false;
	else
		mPlayForward = true;
}

void AnimEvaluator::Evaluate(float time, std::map<std::string, SkinDef::Bone*>& bones)
{
	time *= mTicksPerSec;

	float lTime = 0.0f;
	if(mDuration > 0.0f)
		time = fmod(time, mDuration);

	for(UINT i(0); i != mChannels.size(); ++i)
	{
		const SkinDef::AnimChannel* channel = &mChannels[i];
		std::map<std::string, SkinDef::Bone*>::iterator boneNode = bones.find(channel->mName);

		if(boneNode == bones.end())
			continue;

		// Position
		aiVector3D presentPos(0, 0, 0);
		if(channel->mPosKeys.size() > 0)
		{
			UINT frame = (time >= mLastTime) ? std::get<0>(mLastPositions[i]) : 0;
			while(frame < channel->mPosKeys.size() - 1)
			{
				if(time < channel->mPosKeys[frame+1].mTime)
					break;
				frame++;
			}
			UINT nextFrame = (frame + 1) % channel->mPosKeys.size();

			const aiVectorKey& key = channel->mPosKeys[frame];
			const aiVectorKey& nextKey = channel->mPosKeys[nextFrame];

			double diffTime = nextKey.mTime - key.mTime;

			if(diffTime < 0.0)
				diffTime += mDuration;

			if(diffTime > 0.0)
			{
				float factor = float((time - key.mTime) / diffTime);
				presentPos = key.mValue + (nextKey.mValue - key.mValue) * factor;
			}
			else
				presentPos = key.mValue;

			std::get<0>(mLastPositions[i]) = frame;
		}

		// Rotation
		aiQuaternion presentRot(1, 0, 0, 0);
		if(channel->mRotKeys.size() > 0)
		{
			UINT frame = (time >= mLastTime) ? std::get<1>(mLastPositions[i]) : 0;

			while(frame < channel->mRotKeys.size() - 1)
			{
				if(time < channel->mRotKeys[frame+1].mTime)
					break;
				frame++;
			}

			UINT nextFrame = (frame + 1) % channel->mRotKeys.size();
			const aiQuatKey& key = channel->mRotKeys[frame];
			const aiQuatKey& nextKey = channel->mRotKeys[nextFrame];

			double diffTime = nextKey.mTime - key.mTime;

			if(diffTime < 0.0)
				diffTime += mDuration;

			if(diffTime > 0)
			{
				float factor = float((time - key.mTime) / diffTime);
				aiQuaternion::Interpolate(presentRot, key.mValue, nextKey.mValue, factor);
			}
			else
				presentRot = key.mValue;

			std::get<1>(mLastPositions[i]) = frame;
		}

		// Scaling
		aiVector3D presentScale(1, 1, 1);
		if(channel->mScaleKeys.size() > 0)
		{
			UINT frame = (time >= mLastTime) ? std::get<2>(mLastPositions[i]) : 0;

			while(frame < channel->mScaleKeys.size() - 1)
			{
				if(time < channel->mScaleKeys[frame+1].mTime)
					break;
				frame++;
			}
		}

		aiMatrix4x4 mat = aiMatrix4x4(presentRot.GetMatrix());
		mat.a1 *= presentScale.x; mat.b1 *= presentScale.x; mat.c1 *= presentScale.x;
		mat.a2 *= presentScale.y; mat.b2 *= presentScale.y; mat.c2 *= presentScale.y;
		mat.a3 *= presentScale.z; mat.b3 *= presentScale.z; mat.c3 *= presentScale.z;
		mat.a4 *= presentScale.x; mat.b4 *= presentScale.y; mat.c4 *= presentScale.z;
		mat.Transpose();

		boneNode->second->mLocalTrans = SkinDef::ReadAiMatrix(mat);
	}

	mLastTime = time;
}

UINT AnimEvaluator::CalcFrameIndexAt(float time)
{
	time *= mTicksPerSec;
	float lTime = 0.0f;

	if(mDuration > 0.0f)
		lTime = fmod(time, mDuration);

	float percent = lTime/mDuration;

	if(!mPlayForward)
		percent = (percent-1.0f) * -1.0f;

	UINT frameIndex = static_cast<UINT>((static_cast<float>(mTransforms.size()) * percent));
	return frameIndex;
}

UINT AnimEvaluator::CalcFrameIndexAt(float time, UINT frameStart, UINT frameEnd)
{
	UINT nrOfFrames = (frameEnd - frameStart) + 1;
	float framesDuration = (float)nrOfFrames * (mDuration / static_cast<float>(mTransforms.size()));

	time *= mTicksPerSec;

	float lTime = 0.0f;
	if(framesDuration > 0.0f)
		lTime = fmod(time, framesDuration);

	float percent = lTime / framesDuration;

	if(!mPlayForward)
		percent = (percent-1.0f) * -1.0f;

	UINT frameIndex = static_cast<UINT>((static_cast<float>(nrOfFrames) * percent));
	frameIndex += (frameStart - 1);

	return frameIndex;
}

UINT AnimEvaluator::CalcFrameIndexAt(float time, UINT frameStart, UINT frameEnd, bool playForward)
{
	UINT nrOfFrames = (frameEnd - frameStart) + 1;
	float framesDuration = (float)nrOfFrames * (mDuration / static_cast<float>(mTransforms.size()));

	time *= mTicksPerSec;

	float lTime = 0.0f;
	if(framesDuration > 0.0f)
		lTime = fmod(time, framesDuration);

	float percent = lTime / framesDuration;

	if(!playForward)
		percent = (percent-1.0f) * -1.0f;

	UINT frameIndex = static_cast<UINT>((static_cast<float>(nrOfFrames) * percent));
	frameIndex += (frameStart - 1);

	return frameIndex;
}