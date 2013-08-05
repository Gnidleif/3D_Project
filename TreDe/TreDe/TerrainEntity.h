#pragma once
#ifndef TERRAINENTITY_H
#define TERRAINENTITY_H
#include "VirtualEntity.h"
#include "InputLayouts.h"
#include "Effects.h"

class TerrainEntity : public VirtualEntity
{
public:
	TerrainEntity(std::string key);
	~TerrainEntity(void);
	void Initialize(XMFLOAT3 position, float scale);
	void Draw(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera);
	void DrawTess(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera);

private:
	void CalcWorld();

private:
	TerrainModel::Instance mModelInstance;
};
#endif