#pragma once
#ifndef LIGHTHANDLER_H
#define LIGHTHANDLER_H
#include "LightDef.h"
#include "ShadowMap.h"
#include "StaticEntity.h"

// Very hardcoded class used to handle them lights and stuff!

class LightHandler
{
public:
	LightHandler();
	~LightHandler();
	void Update(float dt);
	void Draw(ID3D11DeviceContext* devCon, Camera* camera);
	void ApplyEffects();

public:
	PointLight GetPoint0() const { return this->mPoints[0]; }
	PointLight GetPoint1() const { return this->mPoints[1]; }

	ID3D11ShaderResourceView* GetDepthMap() const { return this->mShadowMap->GetDepthMapSRV(); }

private:
	int mDirAmount;
	int mPointAmount;
	int mSpotAmount;

	DirectionalLight* mDirs;
	PointLight* mPoints;
	SpotLight* mSpots;

	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;

	ShadowMap* mShadowMap;
};
#endif