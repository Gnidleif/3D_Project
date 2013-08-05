#pragma once
#ifndef LIGHTHANDLER_H
#define LIGHTHANDLER_H
#include "LightDef.h"
#include "ShadowMap.h"
#include "Camera.h"

// Very hardcoded class used to handle them lights and stuff!

class LightHandler
{
public:
	LightHandler();
	~LightHandler();
	void Initialize(ID3D11Device* device);
	void Update(float dt);
	void Draw(Camera* camera);
	void ApplyEffects();
	void ApplyTessEffects();

public:
	XMFLOAT3 GetPoint0Pos() const { return this->mPoints[0].Position; }
	XMFLOAT3 GetPoint1Pos() const { return this->mPoints[1].Position; }

private:
	XMMATRIX CalcView(Camera* camera);
	XMMATRIX CalcProj(Camera* camera);

private:
	ShadowMap* mShadowMap;

	int mDirAmount;
	int mPointAmount;
	int mSpotAmount;

	DirectionalLight* mDirs;
	PointLight* mPoints;
	SpotLight* mSpots;
};
#endif