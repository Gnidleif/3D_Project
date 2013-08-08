#pragma once
#ifndef STATICENTITY_H
#define STATICENTITY_H
#include "VirtualEntity.h"
#include "InputLayouts.h"
#include "Effects.h"

class StaticEntity : public VirtualEntity
{
public:
	StaticEntity(std::string key, std::string texPath);
	~StaticEntity(void);
	void Initialize(XMFLOAT3 position, float scale);
	void Draw(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera);
	void DrawTess(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera);

public:
	StaticModel::Instance* GetInstance() { return this->mModelInstance; }

private:
	void CalcWorld();

private:
	StaticModel::Instance* mModelInstance;
};
#endif

