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
	void UpdateViewProj(Camera* camera);
	void ApplyEffects();

public:
	PointLight GetPoint0() const { return this->mPoints[0]; }
	PointLight GetPoint1() const { return this->mPoints[1]; }

	XMMATRIX GetLightView() const { return XMLoadFloat4x4(&mLightView); }
	XMMATRIX GetLightProj() const { return XMLoadFloat4x4(&mLightProj); }

private:
	static const int mDirAmount = 1;
	static const int mPointAmount = 2;
	static const int mSpotAmount = 2;

	DirectionalLight mDirs[mDirAmount];
	PointLight mPoints[mPointAmount];
	SpotLight mSpots[mSpotAmount];

	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
};
#endif