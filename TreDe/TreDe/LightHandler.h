#ifndef LIGHTHANDLER_H
#define LIGHTHANDLER_H
#include "LightDef.h"
#include "Effects.h"

// Very hardcoded class used to handle them lights and stuff!

class LightHandler
{
public:
	LightHandler();
	~LightHandler();
	void Update(float dt);
	void ApplyEffects();

public:
	XMFLOAT3 GetSpot0Pos() const { return this->mSpots[0].Position; }
	XMFLOAT3 GetSpot1Pos() const { return this->mSpots[1].Position; }

private:
	static const int mDirAmount = 1;
	static const int mPointAmount = 2;
	static const int mSpotAmount = 2;

	DirectionalLight mDirs[mDirAmount];
	PointLight mPoints[mPointAmount];
	SpotLight mSpots[mSpotAmount];
};
#endif