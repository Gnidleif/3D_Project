#pragma once
#ifndef SKINNEDENTITY_H
#define SKINNEDENTITY_H
#include "VirtualEntity.h"
#include "InputLayouts.h"
#include "Effects.h"

class SkinnedEntity : public VirtualEntity
{
public:
	SkinnedEntity(std::string key, std::string texPath);
	~SkinnedEntity(void);
	void Initialize(XMFLOAT3 position, float scale);
	void Draw(ID3D11DeviceContext* devcon, ID3DX11EffectTechnique* activeTech, Camera* camera);
	void Update(float dt);
	void SetKeyFrames(UINT start, UINT end) { this->mModelInstance->mStart = start; this->mModelInstance->mEnd = end; }

	SkinnedModel::Instance* GetModelInstance() const { return this->mModelInstance; }

private:
	void CalcWorld();
	
private:
	SkinnedModel::Instance* mModelInstance;
};
#endif